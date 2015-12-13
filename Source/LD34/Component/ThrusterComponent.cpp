// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "ThrusterComponent.h"
#include "Part/BasePart.h"
#include "Grid.h"


// Sets default values for this component's properties
UThrusterComponent::UThrusterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	ThrustPower = 150000;

	EnergyCost = 300;
}


// Called when the game starts
void UThrusterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UThrusterComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	ABasePart* PartParent = Cast<ABasePart>(this->GetOwner());
	AGrid* GridParent = Cast<AGrid>(this->GetAttachmentRootActor());
	
	//UE_LOG(LogTemp, Display, TEXT("%s %s"), PartParent ? *PartParent->GetName() : TEXT("?"), GridParent ? *GridParent->GetName() : TEXT("?"))

	int32 Facing = FMath::RoundToInt(this->GetRelativeTransform().Rotator().Yaw / 90.f);

	if (Facing < 0) Facing += 4;

	//UE_LOG(LogTemp, Display, TEXT("F %s"), *FString::FromInt(Facing));

	// the direction the thruster is facing
	// Forward = 0
	// Right = 1
	// Back = 2
	// Left = 3

	float Power = 0;

	if (PartParent && GridParent && GridParent->CommandCenters)
	{
		FVector relativeLoc(0, 0, 0);

		auto pc = Cast<UPrimitiveComponent>(GridParent->GetRootComponent());

		if (pc)
		{
			//UE_LOG(LogTemp, Display, TEXT("COM %s CENT %s"), *pc->GetCenterOfMass().ToString(), *GridParent->GetTransform().GetLocation().ToString());
			FVector centerOfMassLocalSpace = GridParent->GetTransform().InverseTransformPosition(pc->GetCenterOfMass());

			relativeLoc = PartParent->GetRootComponent()->GetRelativeTransform().GetLocation() - centerOfMassLocalSpace;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Parent doesn't have a valid root component"));
		}

		DrawDebugString(GetWorld(), PartParent->GetActorLocation(), *relativeLoc.ToString(), nullptr, FColor::Green, DeltaTime);

		if (GridParent->ForwardBackwardThrust > 0.1f && Facing == 2) Power = GridParent->ForwardBackwardThrust;
		if (GridParent->ForwardBackwardThrust < -0.1f && Facing == 0) Power = -GridParent->ForwardBackwardThrust;

		if (GridParent->LeftRightThrust > 0.1f && Facing == 3) Power = GridParent->LeftRightThrust;
		if (GridParent->LeftRightThrust < -0.1f && Facing == 1) Power = -GridParent->LeftRightThrust;

		// nose thrusters, left/right
		if (relativeLoc.X > 0)
		{
			if (GridParent->LeftRightTurn > 0.1f && Facing == 3) Power = GridParent->LeftRightTurn;
			if (GridParent->LeftRightTurn < -0.1f && Facing == 1) Power = -GridParent->LeftRightTurn;
		}

		// tail thrusters, left/right
		if (relativeLoc.X < 0)
		{
			if (GridParent->LeftRightTurn > 0.1f && Facing == 1) Power = GridParent->LeftRightTurn;
			if (GridParent->LeftRightTurn < -0.1f && Facing == 3) Power = -GridParent->LeftRightTurn;
		}

		// left thrusters, forward/backward
		if (relativeLoc.Y > 0)
		{
			if (GridParent->LeftRightTurn > 0.1f && Facing == 0) Power = GridParent->LeftRightTurn;
			if (GridParent->LeftRightTurn < -0.1f && Facing == 2) Power = -GridParent->LeftRightTurn;
		}

		// right thrusters, forward/backward
		if (relativeLoc.Y < 0)
		{
			if (GridParent->LeftRightTurn > 0.1f && Facing == 2) Power = GridParent->LeftRightTurn;
			if (GridParent->LeftRightTurn < -0.1f && Facing == 0) Power = -GridParent->LeftRightTurn;
		}
	}

	if (!GridParent) Power = 0;

	if (GridParent && GridParent->Power < EnergyCost * DeltaTime) Power = 0;

	if (Power > 0.1f)
	{
		auto ForceTarget = Cast<UPrimitiveComponent>(PartParent->GetRootComponent());

		if (ForceTarget)
		{
			ForceTarget->AddImpulseAtLocation(this->GetComponentRotation().RotateVector(FVector(-ThrustPower * DeltaTime * Power, 0, 0)), this->GetComponentLocation());
		}

		if (GridParent)
		{
			GridParent->Power -= EnergyCost * DeltaTime;
		}
	}

	TArray<USceneComponent*> children;

	this->GetChildrenComponents(false, children);

	for (auto c : children)
	{
		c->SetVisibility(Power > 0.1f, true);
	}
}

