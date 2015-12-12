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
	
}

// Called every frame
void AGrid::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//UE_LOG(LogTemp, Display, TEXT("Pos %s"), *Destination.ToString());

	//FVector2D delta = (Destination - FVector2D(GetActorLocation().X, GetActorLocation().Y));

	//float angleToTarget = FMath::RadiansToDegrees(FMath::Atan2(delta.Y, delta.X));

	//float curAngle = GetActorRotation().Yaw;

	//float newAngle = FMath::FixedTurn(curAngle, angleToTarget, 1);

	//FMath::FindDeltaAngle

	if (DestinationMode)
	{
		/*float curAngle = FMath::DegreesToRadians(GetActorRotation().Yaw);

		FVector2D pointCenter(GetActorLocation().X, GetActorLocation().Y);

		FVector2D pointLeft = pointCenter + FVector2D(FMath::Cos(curAngle - 0.22f), FMath::Sin(curAngle - 0.22f))*1;
		FVector2D pointMiddle = pointCenter + FVector2D(FMath::Cos(curAngle), FMath::Sin(curAngle)) * 1;
		FVector2D pointRight = pointCenter + FVector2D(FMath::Cos(curAngle + 0.22f), FMath::Sin(curAngle + 0.22f)) * 1;

		//UE_LOG(LogTemp, Display, TEXT("Pos %s"), *Cast<UPrimitiveComponent>(GetRootComponent())->GetPhysicsAngularVelocity().ToString());

		//DrawDebugSphere(GetWorld(), FVector(pointLeft.X, pointLeft.Y, 200), 50, 4, FColor::Red, false, 1);
		//DrawDebugSphere(GetWorld(), FVector(pointMiddle.X, pointMiddle.Y, 200), 50, 4, FColor::Green, false, 1);
		//DrawDebugSphere(GetWorld(), FVector(pointRight.X, pointRight.Y, 200), 50, 4, FColor::Blue, false, 1);

		float leftDist = FVector2D::DistSquared(pointLeft, Destination);
		float middleDist = FVector2D::DistSquared(pointMiddle, Destination);
		float rightDist = FVector2D::DistSquared(pointRight, Destination);

		float desiredAngularVelocity = 0;
		float actualAngularVelocity = Cast<UPrimitiveComponent>(GetRootComponent())->GetPhysicsAngularVelocity().Z;

		if (leftDist < middleDist && leftDist < rightDist)
		{
			desiredAngularVelocity = -120;
		}

		if (rightDist < middleDist && rightDist < leftDist)
		{
			desiredAngularVelocity = 120;
		}*/

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
}

// Called to bind functionality to input
void AGrid::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("ForwardBackward", this, &AGrid::SetForwardBackwardThrust);
	InputComponent->BindAxis("LeftRightStrafe", this, &AGrid::SetLeftRightThrust);
	InputComponent->BindAxis("LeftRightTurn", this, &AGrid::SetLeftRightTurn);
	InputComponent->BindAxis("FiringGroup0", this, &AGrid::SetIsFiringGroup0);
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
			root->AttachTo(myRoot, NAME_None, EAttachLocation::KeepRelativeOffset, true);
			Part->GridX = X;
			Part->GridY = Y;

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

		partApart->DetachFromGrid();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove attempt failed: Grid %s contains nothing at %s, %s"), *GetName(), *FString::FromInt(X), *FString::FromInt(Y));
	}
}