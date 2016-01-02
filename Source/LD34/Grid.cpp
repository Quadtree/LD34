// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "Grid.h"
#include "Part/BasePart.h"
#include "UnrealNetwork.h"


// Sets default values
AGrid::AGrid()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DestinationMode = true;
	bReplicates = true;
	bReplicateMovement = true;
	NetCullDistanceSquared = FMath::Square(100000.f);
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();

	CameraHeight = CameraDefaultHeight;
	CameraDesiredHeight = CameraDefaultHeight;

	if (auto c = FindComponentByClass<UCameraComponent>())
	{
#if PLATFORM_HTML5 || PLATFORM_ANDROID || PLATFORM_IOS || PLATFORM_WINRT
		c->PostProcessSettings.AutoExposureBias = ES2Brightness;
#endif
	}
}

// Called every frame
void AGrid::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (auto a = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		a->SetLinearDamping(0.2f);
		a->SetAngularDamping(0.2f);
	}

	if (DestinationMode && CommandCenters)
	{

		float desiredAngularVelocity = 0;
		float actualAngularVelocity = Cast<UPrimitiveComponent>(GetRootComponent())->GetPhysicsAngularVelocity().Z;

		FVector2D delta = (Destination - FVector2D(GetActorLocation().X, GetActorLocation().Y));

		float angleToTarget = FMath::Atan2(delta.Y, delta.X);

		float curAngle = FMath::DegreesToRadians(GetActorRotation().Yaw);

		float deltaAngle = FMath::FindDeltaAngle(curAngle, angleToTarget);

		desiredAngularVelocity = FMath::Pow(deltaAngle, 1) * 60;

		if (FMath::Abs(actualAngularVelocity - desiredAngularVelocity) > 5)
		{
			if (actualAngularVelocity > desiredAngularVelocity)
			{
				LeftRightTurn = -1;
			}
			else
			{
				LeftRightTurn = 1;
			}
		}
		
	}

	Power += PowerRegenRate * DeltaTime;
	Power = FMath::Clamp(Power, 0.f, MaxPower);

	const float SHIELD_POWER_PER_HP = 4;

	float shieldPowerConsumed = FMath::Max(FMath::Min3((ShieldMax - Shield) * SHIELD_POWER_PER_HP, Power, ShieldRegenRate * SHIELD_POWER_PER_HP * DeltaTime), 0.f);

	Shield += shieldPowerConsumed / SHIELD_POWER_PER_HP;
	Shield = FMath::Clamp(Shield, 0.f, ShieldMax);

	const float MAX_SPEED = 2000;

	if (auto a = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		float speed = a->GetPhysicsLinearVelocity().Size();

		if (speed > MAX_SPEED)
		{
			a->SetPhysicsLinearVelocity(a->GetPhysicsLinearVelocity().GetSafeNormal() * MAX_SPEED);
		}
	}

	if (auto cam = FindComponentByClass<UCameraComponent>())
	{
		const float CAMERA_INTERPOLATE_SPEED = 0.05f;

		//cam->SetRelativeLocation(FVector(0, 0, CameraHeight));
		cam->OrthoWidth = CameraHeight;

		CameraHeight = CameraHeight * (1 - CAMERA_INTERPOLATE_SPEED) + CameraDesiredHeight * CAMERA_INTERPOLATE_SPEED;
	}
}

// Called to bind functionality to input
void AGrid::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("ForwardBackward", this, &AGrid::SetForwardBackwardThrust);
	InputComponent->BindAxis("LeftRightStrafe", this, &AGrid::SetLeftRightThrust);
	InputComponent->BindAxis("LeftRightTurn", this, &AGrid::SetLeftRightTurn);
	InputComponent->BindAxis("FiringGroup0", this, &AGrid::SetIsFiringGroup0);

	InputComponent->BindAction("ZoomIn", EInputEvent::IE_Pressed, this, &AGrid::ZoomIn);
	InputComponent->BindAction("ZoomOut", EInputEvent::IE_Pressed, this, &AGrid::ZoomOut);
}

void AGrid::ZoomIn()
{
	//UE_LOG(LogTemp, Display, TEXT("Zoom in"));

	CameraDesiredHeight = FMath::Clamp(CameraDesiredHeight - CameraMovespeed, MinCameraHeight, MaxCameraHeight);
}

void AGrid::ZoomOut()
{
	//UE_LOG(LogTemp, Display, TEXT("Zoom out"));

	CameraDesiredHeight = FMath::Clamp(CameraDesiredHeight + CameraMovespeed, MinCameraHeight, MaxCameraHeight);
}

void AGrid::SetForwardBackwardThrust(float val)
{
	auto old = this->ForwardBackwardThrust;
	this->ForwardBackwardThrust = val;

	if (Role != ROLE_Authority && old != this->ForwardBackwardThrust) ServerSetForwardBackwardThrust(val);
}

bool AGrid::ServerSetForwardBackwardThrust_Validate(float val)
{
	return FMath::Abs(val) < 1.01f;
}

void AGrid::ServerSetForwardBackwardThrust_Implementation(float val)
{
	SetForwardBackwardThrust(val);
}

bool AGrid::ServerSetLeftRightThrust_Validate(float val)
{
	return FMath::Abs(val) < 1.01f;
}

void AGrid::SetLeftRightThrust(float val)
{
	auto old = this->LeftRightThrust;
	this->LeftRightThrust = val;

	if (Role != ROLE_Authority && old != this->LeftRightThrust) ServerSetLeftRightThrust(val);
}

void AGrid::ServerSetLeftRightThrust_Implementation(float val)
{
	SetLeftRightThrust(val);
}

void AGrid::SetLeftRightTurn(float val)
{
	this->LeftRightTurn = val;
}

void AGrid::SetIsFiringGroup0(float val)
{
	auto old = this->IsFiringGroup0;

	this->IsFiringGroup0 = val > 0.5f;

	if (Role != ROLE_Authority && old != this->IsFiringGroup0) ServerSetIsFiringGroup0(val);
}

bool AGrid::ServerSetIsFiringGroup0_Validate(float val)
{
	return true;
}

void AGrid::ServerSetIsFiringGroup0_Implementation(float val)
{
	SetIsFiringGroup0(val);
}

float AGrid::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float ret = 0;

	if (Role == ENetRole::ROLE_Authority)
	{
		if (Shield >= DamageAmount)
		{
			Shield -= DamageAmount;
			return DamageAmount;
		}
	
		if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{
			FPointDamageEvent const & pt = (FPointDamageEvent const &) DamageEvent;

			FVector localSpace = this->GetTransform().InverseTransformPosition(pt.HitInfo.ImpactPoint);

			//UE_LOG(LogTemp, Display, TEXT("HIT PT %s %s"), *pt.HitInfo.ImpactPoint.ToString(), *localSpace.ToString());

			int32 gridX = FMath::RoundToInt(localSpace.X / 100);
			int32 gridY = FMath::RoundToInt(localSpace.Y / 100);

			if (Cells.Contains(gridX) && Cells[gridX].Contains(gridY) && Cells[gridX][gridY])
			{
				ret = Cells[gridX][gridY]->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("No part to hit at %s, %s"), *FString::FromInt(gridX), *FString::FromInt(gridY));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Unrecognized class id %s"), *FString::FromInt(DamageEvent.ClassID));
		}
	}

	return ret;
}

void AGrid::ContinuityCheck()
{
	if (Role != ENetRole::ROLE_Authority) return;

	TArray<TArray<ABasePart*>> distinctGrids;

	TSet<ABasePart*> covered;
	TSet<ABasePart*> notCovered;

	for (auto i : Cells)
	{
		for (auto j : i.Value)
		{
			if (j.Value && j.Value->IsValidLowLevel())
			{
				notCovered.Add(j.Value);
			}
		}
	}

	if (notCovered.Num() == 0 && !Cast<APlayerController>(GetController()))
	{
		Destroy();
		return;
	}

	for (auto v : notCovered)
	{
		if (covered.Contains(v)) continue;

		TArray<ABasePart*> attached = GetAllContiguous(v);

		covered.Append(attached);

		if (attached.Num() == 1)
		{
			// there is only one component in this, so just send it flipping
			ABasePart* r = this->GetPartAt(v->GridX, v->GridY);
			this->RemoveAt(v->GridX, v->GridY);
			r->GoFlipping();
		}
		else
		{
			distinctGrids.Add(attached);
		}
	}

	distinctGrids.Sort([](const TArray<ABasePart*> a, const TArray<ABasePart*> b) { return a.Num() > b.Num(); });

	// for any grids ABOVE 0 we need to create new grids
	for (int32 i = 1; i < distinctGrids.Num(); ++i)
	{
		AGrid* newGrid = GetWorld()->SpawnActor<AGrid>(BaseGridType, GetActorLocation(), GetActorRotation());

		if (newGrid)
		{
			UE_LOG(LogTemp, Display, TEXT("Split! New grid has %s parts, grid 0 has %s parts"), *FString::FromInt(distinctGrids[i].Num()), *FString::FromInt(distinctGrids[0].Num()));

			newGrid->SpawnDefaultController();
			newGrid->Faction = Faction;

			for (auto a : distinctGrids[i])
			{
				this->RemoveAt(a->GridX, a->GridY);
				newGrid->AddToGrid(a, a->GridX, a->GridY);
			}
		}
	}
}

TArray<ABasePart*> AGrid::GetAllContiguous(ABasePart* start)
{
	TSet<ABasePart*> closed;
	TSet<ABasePart*> open;

	open.Add(start);

	while (open.Num() > 0)
	{
		// get some node. order does not matter for this algorithm
		ABasePart* node = *open.CreateIterator();

		if (auto n = GetPartAt(node->GridX + 1, node->GridY)) if (!open.Contains(n) && !closed.Contains(n)) open.Add(n);
		if (auto n = GetPartAt(node->GridX - 1, node->GridY)) if (!open.Contains(n) && !closed.Contains(n)) open.Add(n);
		if (auto n = GetPartAt(node->GridX, node->GridY + 1)) if (!open.Contains(n) && !closed.Contains(n)) open.Add(n);
		if (auto n = GetPartAt(node->GridX, node->GridY - 1)) if (!open.Contains(n) && !closed.Contains(n)) open.Add(n);

		open.Remove(node);
		closed.Add(node);
	}
	

	return closed.Array();
}

ABasePart* AGrid::GetPartAt(int32 X, int32 Y)
{
	if (Cells.Contains(X) && Cells[X].Contains(Y) && Cells[X][Y] && Cells[X][Y]->IsValidLowLevel())
		return Cells[X][Y];
	else
		return nullptr;
}

void AGrid::AddToGrid_Implementation(class ABasePart* Part, int32 X, int32 Y)
{
	if (Part)
	{
		USceneComponent* root = Part->GetRootComponent();
		USceneComponent* myRoot = this->GetRootComponent();

		if (root && myRoot)
		{
			if (Cells.Contains(X) && Cells[X].Contains(Y) && Cells[X][Y])
			{
				UE_LOG(LogTemp, Warning, TEXT("Grid %s already contains a part at %s, %s"), *GetName(), *FString::FromInt(X), *FString::FromInt(Y));
			}

			root->SetRelativeLocation(FVector(X * 100, Y * 100, 0));
			root->SetRelativeRotation(FRotator::ZeroRotator);
			root->AttachTo(myRoot, NAME_None, EAttachLocation::KeepRelativeOffset, true);
			Part->GridX = X;
			Part->GridY = Y;

			if (Part->IsCommandCenter) CommandCenters++;

			MaxPower += Part->MaxPowerMod;
			PowerRegenRate += Part->PowerRegenMod;
			ShieldMax += Part->ShieldMaxMod;
			ShieldRegenRate += Part->ShieldRegenMod;

			auto p = Cast<UPrimitiveComponent>(myRoot);

			if (p)
			{
				p->GetBodyInstance()->UpdateMassProperties();

				//UE_LOG(LogTemp, Display, TEXT("%s mass is now %s"), *p->GetName(), *FString::SanitizeFloat(p->GetMass()));
			}

			if (!Cells.Contains(X)) Cells.Add(X);
			if (!Cells[X].Contains(Y)) Cells[X].Add(Y);

			Cells[X][Y] = Part;
			Part->ExpectedParentGrid = this;

			RecalculateBounds();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No root component found in %s or %s"), *Part->GetName(), *this->GetName());
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s Part passed in was null"), *FString::FromInt(this->Role));
	}
}

void AGrid::CreateAndAddToGrid(TSubclassOf<class ABasePart> Part, int32 X, int32 Y)
{
	ABasePart* NewPart = GetWorld()->SpawnActor<ABasePart>(Part);

	if (NewPart)
	{
		//UE_LOG(LogTemp, Display, TEXT("New part %s created"), *Part->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to create new part"));
	}
	

	AddToGrid(NewPart, X, Y);
}

void AGrid::RemoveAt(int32 X, int32 Y)
{
	if (Cells.Contains(X) && Cells[X].Contains(Y) && Cells[X][Y] && Cells[X][Y]->IsValidLowLevel())
	{
		ABasePart* partApart = Cells[X][Y];
		Cells[X][Y] = nullptr;

		if (partApart->IsCommandCenter) CommandCenters--;

		MaxPower -= partApart->MaxPowerMod;
		PowerRegenRate -= partApart->PowerRegenMod;
		ShieldMax -= partApart->ShieldMaxMod;
		ShieldRegenRate -= partApart->ShieldRegenMod;

		partApart->DetachFromGrid();

		RecalculateBounds();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove attempt failed: Grid %s contains nothing at %s, %s"), *GetName(), *FString::FromInt(X), *FString::FromInt(Y));
	}
}

void AGrid::OnHitHandler(AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (!CommandCenters || Role != ROLE_Authority) return;

	if (auto bp = Cast<ABasePart>(OtherActor))
	{
		// make sure this component is unattached...
		if (!bp->GetAttachParentActor() && (bp->GridLockTime <= 0 || bp->LockedGrid != GetName()))
		{
			FVector localSpace = this->GetTransform().InverseTransformPosition(bp->GetActorLocation());

			int32 gridX = FMath::RoundToInt(localSpace.X / 100);
			int32 gridY = FMath::RoundToInt(localSpace.Y / 100);

			TArray<FIntPoint> possiblePoints;
			possiblePoints.Add(FIntPoint(gridX, gridY));
			
			TArray<FIntPoint> edgePoints;

			for (int32 x = -1; x <= 1; ++x) for (int32 y = -1; y <= 1; ++y) edgePoints.Add(possiblePoints[0] + FIntPoint(x, y));

			while (edgePoints.Num() > 0)
			{
				int32 n = FMath::Rand() % edgePoints.Num();

				possiblePoints.Add(edgePoints[n]);
				edgePoints.RemoveAt(n);
			}

			bool anyToConnectTo = false;

			for (auto a : possiblePoints)
			{
				gridX = a.X;
				gridY = a.Y;

				if (GetPartAt(gridX, gridY)) continue;

				if (GetPartAt(gridX + 1, gridY)) anyToConnectTo = true;
				if (GetPartAt(gridX - 1, gridY)) anyToConnectTo = true;
				if (GetPartAt(gridX, gridY + 1)) anyToConnectTo = true;
				if (GetPartAt(gridX, gridY - 1)) anyToConnectTo = true;

				if (anyToConnectTo) break;
			}

			if (anyToConnectTo)
			{
				AddToGrid(bp, gridX, gridY);

				if (AddPartSound) UGameplayStatics::PlaySoundAtLocation(this, AddPartSound, GetActorLocation());

				ContinuityCheck();
			}
		}
	}
}

float AGrid::GetPowerPct()
{
	if (MaxPower > 0.1f)
	{
		return Power / MaxPower;
	}
	else
	{
		return 0;
	}
}

void AGrid::RecalculateBounds()
{
	BoundingSphereCenter = FVector(0, 0, 0);
	auto pc = Cast<UPrimitiveComponent>(this->GetRootComponent());

	if (pc)
	{
		BoundingSphereCenter = this->GetTransform().InverseTransformPosition(pc->GetCenterOfMass());
	}

	BoundingSphereRadius = 0;

	for (auto i : Cells)
	{
		for (auto j : i.Value)
		{
			if (j.Value && j.Value->IsValidLowLevel())
			{
				BoundingSphereRadius = FMath::Max(BoundingSphereRadius, FVector::Dist(BoundingSphereCenter, FVector(i.Key * 100.f, j.Key * 100.f, 0)) + 71);
			}
		}
	}
}

float AGrid::GetShieldPct()
{
	if (ShieldMax > 0)
		return Shield / (3.14159f * FMath::Square(BoundingSphereRadius)) * 500;
	else
		return 0;
}

void AGrid::SetDestination(FVector2D destination)
{
	auto old = this->Destination;
	this->Destination = destination;

	if (old != destination) this->ServerSetDestination(destination);
}

bool AGrid::ServerSetDestination_Validate(FVector2D destination)
{
	return true;
}

void AGrid::ServerSetDestination_Implementation(FVector2D destination)
{
	this->Destination = destination;
}

void AGrid::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGrid, Shield);
	DOREPLIFETIME(AGrid, Power);
	DOREPLIFETIME(AGrid, Faction);
	DOREPLIFETIME(AGrid, ForwardBackwardThrust);
	DOREPLIFETIME(AGrid, LeftRightThrust);
	DOREPLIFETIME(AGrid, IsFiringGroup0);
}