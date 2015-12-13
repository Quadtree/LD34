// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "BasePart.h"
#include "Grid.h"


// Sets default values
ABasePart::ABasePart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OddsOfSurvival = 1;
}

// Called when the game starts or when spawned
void ABasePart::BeginPlay()
{
	Super::BeginPlay();
	
	MaxHealth = Health;

	auto a = Cast<UPrimitiveComponent>(GetRootComponent());

	if (a)
	{
		a->OnComponentHit.AddUniqueDynamic(this, &ABasePart::OnHitHandler);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s No real root component"), *GetName());
	}
}

// Called every frame
void ABasePart::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	GridLockTime -= DeltaTime;
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
	if (FMath::FRand() <= OddsOfSurvival)
	{
		auto p = Cast<UPrimitiveComponent>(GetRootComponent());

		if (p)
		{
			const float FLIP_LINEAR_FORCE = 10;
			const float FLIP_ANGULAR_FORCE = 3;

			p->AddImpulse(FMath::RandPointInBox(FBox(FVector(-FLIP_LINEAR_FORCE, -FLIP_LINEAR_FORCE, 0), FVector(FLIP_LINEAR_FORCE, FLIP_LINEAR_FORCE, 0))), NAME_None, true);
			p->AddAngularImpulse(FMath::RandPointInBox(FBox(FVector(-FLIP_ANGULAR_FORCE, -FLIP_ANGULAR_FORCE, -FLIP_ANGULAR_FORCE), FVector(FLIP_ANGULAR_FORCE, FLIP_ANGULAR_FORCE, FLIP_ANGULAR_FORCE))), NAME_None, true);
		}
	}
	else
	{
		Destroy();
	}
}

void ABasePart::OnHitHandler(AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	auto g = Cast<AGrid>(GetRootComponent()->GetAttachmentRootActor());

	if (g)
	{
		g->OnHitHandler(OtherActor, OtherComp, NormalImpulse, Hit);
	}
}
