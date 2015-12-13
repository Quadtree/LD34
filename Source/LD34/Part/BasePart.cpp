// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "BasePart.h"
#include "Grid.h"


// Sets default values
ABasePart::ABasePart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABasePart::BeginPlay()
{
	Super::BeginPlay();
	
	MaxHealth = Health;
}

// Called every frame
void ABasePart::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

float ABasePart::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ret = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= ret;

	if (Health <= 0)
	{
		if (auto g = Cast<AGrid>(GetRootComponent()->GetAttachmentRootActor()))
		{
			g->RemoveAt(GridX, GridY);

			GoFlipping();

			g->ContinuityCheck();
		}

		Health = MaxHealth;
	}

	UE_LOG(LogTemp, Display, TEXT("%s took %s damage health is now %s"), *GetName(), *FString::SanitizeFloat(DamageAmount), *FString::SanitizeFloat(Health));

	return ret;
}

void ABasePart::DetachFromGrid()
{
	AGrid* parentGrid = Cast<AGrid>(GetRootComponent()->GetAttachmentRootActor());

	GetRootComponent()->DetachFromParent(true);

	GridLockTime = 5;
	LockedGrid = parentGrid->GetName();
}

void ABasePart::GoFlipping()
{
	auto p = Cast<UPrimitiveComponent>(GetRootComponent());

	if (p)
	{
		p->AddImpulse(FMath::RandPointInBox(FBox(FVector(-1000, -1000, 0), FVector(1000, 1000, 0))), NAME_None, true);
		p->AddAngularImpulse(FMath::RandPointInBox(FBox(FVector(-100, -100, -100), FVector(100, 100, 100))), NAME_None, true);
	}
}
