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

	// ...
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

	int32 Facing = FMath::Round(this->GetRelativeTransform().Rotator().Yaw / 90.f);

	if (Facing < 0) Facing += 4;

	//UE_LOG(LogTemp, Display, TEXT("F %s"), *FString::FromInt(Facing));

	// the direction the thruster is facing
	// Forward = 0
	// Right = 1
	// Back = 2
	// Left = 3

	float Power = 0;

	if (PartParent && GridParent)
	{
		if (GridParent->ForwardBackwardThrust > 0.1f && Facing == 2) Power = GridParent->ForwardBackwardThrust;
		if (GridParent->ForwardBackwardThrust < -0.1f && Facing == 0) Power = -GridParent->ForwardBackwardThrust;
	}

	if (Power > 0.1f)
	{
		auto ForceTarget = Cast<UPrimitiveComponent>(PartParent->GetRootComponent());

		if (ForceTarget)
		{
			ForceTarget->AddImpulseAtLocation(this->GetComponentRotation().RotateVector(FVector(1000 * Power, 0, 0)), this->GetComponentLocation());
		}
	}
}

