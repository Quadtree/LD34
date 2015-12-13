// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "Grid.h"
#include "Part/BasePart.h"


// Sets default values
AGrid::AGrid()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DestinationMode = true;
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
	
	auto a = Cast<UPrimitiveComponent>(GetRootComponent());

	if (a)
	{
		a->OnComponentHit.AddUniqueDynamic(this, &AGrid::OnHitHandler);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s No real root component"), *GetName());
	}

	CameraHeight = CameraDefaultHeight;
	CameraDesiredHeight = CameraDefaultHeight;
}

// Called every frame
void AGrid::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (DestinationMode && CommandCenters)
	{

		float desiredAngularVelocity = 0;
		float actualAngularVelocity = Cast<UPrimitiveComponent>(GetRootComponent())->GetPhysicsAngularVelocity().Z;

		FVector2D delta = (Destination - FVector2D(GetActorLocation().X, GetActorLocation().Y));

		float angleToTarget = FMath::Atan2(delta.Y, delta.X);

		float curAngle = FMath::DegreesToRadians(GetActorRotation().Yaw);

		float deltaAngle = FMath::FindDeltaAngle(curAngle, angleToTarget);

		desiredAngularVelocity = FMath::Pow(deltaAngle, 1) * 200;

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

		cam->SetRelativeLocation(FVector(0, 0, CameraHeight));

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
	UE_LOG(LogTemp, Display, TEXT("Zoom in"));

	CameraDesiredHeight = FMath::Clamp(CameraDesiredHeight - CameraMovespeed, MinCameraHeight, MaxCameraHeight);
}

void AGrid::ZoomOut()
{
	UE_LOG(LogTemp, Display, TEXT("Zoom out"));

	CameraDesiredHeight = FMath::Clamp(CameraDesiredHeight + CameraMovespeed, MinCameraHeight, MaxCameraHeight);
}

void AGrid::SetForwardBackwardThrust(float val)
{
	this->ForwardBackwardThrust = val;
}

void AGrid::SetLeftRightThrust(float val)
{
	this->LeftRightThrust = val;
}

void AGrid::SetLeftRightTurn(float val)
{
	this->LeftRightTurn = val;
}

void AGrid::SetIsFiringGroup0(float val)
{
	this->IsFiringGroup0 = val > 0.5f;
}

float AGrid::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float ret = 0;

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent const & pt = (FPointDamageEvent const &) DamageEvent;

		FVector localSpace = this->GetTransform().InverseTransformPosition(pt.HitInfo.ImpactPoint);

		UE_LOG(LogTemp, Display, TEXT("HIT PT %s %s"), *pt.HitInfo.ImpactPoint.ToString(), *localSpace.ToString());

		int32 gridX = FMath::RoundToInt(localSpace.X / 100);
		int32 gridY = FMath::RoundToInt(localSpace.Y / 100);

		if (Cells.Contains(gridX) && Cells[gridX].Contains(gridY) && Cells[gridX][gridY])
		{
			ret = Cells[gridX][gridY]->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No part to hit at %s, %s"), *FString::FromInt(gridX), *FString::FromInt(gridY));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unrecognized class id %s"), *FString::FromInt(DamageEvent.ClassID));
	}

	return ret;
}

void AGrid::ContinuityCheck()
{
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

void AGrid::AddToGrid(class ABasePart* Part, int32 X, int32 Y)
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

			auto p = Cast<UPrimitiveComponent>(myRoot);

			if (p)
			{
				p->GetBodyInstance()->UpdateMassProperties();

				UE_LOG(LogTemp, Display, TEXT("%s mass is now %s"), *p->GetName(), *FString::SanitizeFloat(p->GetMass()));
			}

			if (!Cells.Contains(X)) Cells.Add(X);
			if (!Cells[X].Contains(Y)) Cells[X].Add(Y);

			Cells[X][Y] = Part;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No root component found in %s or %s"), *Part->GetName(), *this->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Part passed in was null"));
	}
}

void AGrid::CreateAndAddToGrid(TSubclassOf<class ABasePart> Part, int32 X, int32 Y)
{
	ABasePart* NewPart = GetWorld()->SpawnActor<ABasePart>(Part);

	if (NewPart)
	{
		UE_LOG(LogTemp, Display, TEXT("New part %s created"), *Part->GetName());
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

		MaxPower += partApart->MaxPowerMod;
		PowerRegenRate += partApart->PowerRegenMod;

		partApart->DetachFromGrid();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove attempt failed: Grid %s contains nothing at %s, %s"), *GetName(), *FString::FromInt(X), *FString::FromInt(Y));
	}
}

void AGrid::OnHitHandler(AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (auto bp = Cast<ABasePart>(OtherActor))
	{
		// make sure this component is unattached...
		if (!bp->GetAttachParentActor() && (bp->GridLockTime <= 0 || bp->LockedGrid != GetName()))
		{
			FVector localSpace = this->GetTransform().InverseTransformPosition(bp->GetActorLocation());

			int32 gridX = FMath::RoundToInt(localSpace.X / 100);
			int32 gridY = FMath::RoundToInt(localSpace.Y / 100);

			AddToGrid(bp, gridX, gridY);

			ContinuityCheck();
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
