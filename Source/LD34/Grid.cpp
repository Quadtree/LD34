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

	if (DestinationMode)
	{
		float curAngle = FMath::DegreesToRadians(GetActorRotation().Yaw);

		FVector2D pointCenter(GetActorLocation().X, GetActorLocation().Y);

		FVector2D pointLeft = pointCenter + FVector2D(FMath::Cos(curAngle - 0.22f), FMath::Sin(curAngle - 0.22f))*200;
		FVector2D pointMiddle = pointCenter + FVector2D(FMath::Cos(curAngle), FMath::Sin(curAngle)) * 200;
		FVector2D pointRight = pointCenter + FVector2D(FMath::Cos(curAngle + 0.22f), FMath::Sin(curAngle + 0.22f)) * 200;

		UE_LOG(LogTemp, Display, TEXT("Pos %s"), *pointMiddle.ToString());

		DrawDebugSphere(GetWorld(), FVector(pointLeft.X, pointLeft.Y, 200), 50, 4, FColor::Red, false, 1);
		DrawDebugSphere(GetWorld(), FVector(pointMiddle.X, pointMiddle.Y, 200), 50, 4, FColor::Green, false, 1);
		DrawDebugSphere(GetWorld(), FVector(pointRight.X, pointRight.Y, 200), 50, 4, FColor::Blue, false, 1);

		float leftDist = FVector2D::DistSquared(pointLeft, Destination);
		float middleDist = FVector2D::DistSquared(pointMiddle, Destination);
		float rightDist = FVector2D::DistSquared(pointRight, Destination);

		if (leftDist < middleDist && leftDist < rightDist)
		{
			LeftRightTurn = -1;
		}

		if (rightDist < middleDist && rightDist < leftDist)
		{
			LeftRightTurn = 1;
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

void AGrid::AddToGrid(class ABasePart* Part, int32 X, int32 Y)
{
	if (Part)
	{
		USceneComponent* root = Part->GetRootComponent();
		USceneComponent* myRoot = this->GetRootComponent();

		if (root && myRoot)
		{
			root->SetRelativeLocation(FVector(X * 100, Y * 100, 0));
			root->AttachTo(myRoot, NAME_None, EAttachLocation::KeepRelativeOffset, true);
			Part->GridX = X;
			Part->GridY = Y;
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