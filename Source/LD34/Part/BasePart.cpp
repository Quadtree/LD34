// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "BasePart.h"
#include "Grid.h"
#include "UnrealNetwork.h"


// Sets default values
ABasePart::ABasePart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OddsOfSurvival = .4f;
	bReplicates = true;
	bReplicateMovement = true;
	NetPriority = 1;
	NetUpdateFrequency = 1500;
}

// Called when the game starts or when spawned
void ABasePart::BeginPlay()
{
	Super::BeginPlay();
	
	MaxHealth = Health;

	auto a = Cast<UPrimitiveComponent>(GetRootComponent());

	if (a)
	{
		//if (!a->OnComponentHit.IsAlreadyBound(this, &ABasePart::OnHitHandler))
		//	a->OnComponentHit.AddUniqueDynamic(this, &ABasePart::OnHitHandler);

		a->SetLinearDamping(0.5f);
		a->SetAngularDamping(2);
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

	AGrid* parentGrid = Cast<AGrid>(GetRootComponent()->GetAttachmentRootActor());

	if (parentGrid && !ExpectedParentGrid)
	{
		// we're attached to something but the server says we shouldn't be
		DetachFromGrid();
	}

	if (ExpectedParentGrid && !parentGrid)
	{
		// the server says we're attached to something but we don't seem to be
		ExpectedParentGrid->AddToGrid(this, GridX, GridY);
	}

	if (parentGrid)
	{
		LooseTime = 0;
	}
	else
	{
		LooseTime += DeltaTime;

		if (LooseTime > 120 && Role == ROLE_Authority)
		{
			Destroy();
		}
	}
}

float ABasePart::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ret = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Role == ROLE_Authority)
	{
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

		//UE_LOG(LogTemp, Display, TEXT("%s took %s damage health is now %s"), *GetName(), *FString::SanitizeFloat(DamageAmount), *FString::SanitizeFloat(Health));
	}

	return ret;
}

void ABasePart::DetachFromGrid_Implementation()
{
	AGrid* parentGrid = Cast<AGrid>(GetRootComponent()->GetAttachmentRootActor());

	if (parentGrid)
	{
		GetRootComponent()->DetachFromParent(true);

		GridLockTime = 5;
		LockedGrid = parentGrid->GetName();
	}

	ExpectedParentGrid = nullptr;
}

void ABasePart::GoFlipping_Implementation()
{
	if (ExplosionSound) UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

	if (Role == ROLE_Authority)
	{
		if (FMath::FRand() <= OddsOfSurvival)
		{
			auto p = Cast<UPrimitiveComponent>(GetRootComponent());

			if (p)
			{
				const float FLIP_LINEAR_FORCE = 600;
				const float FLIP_ANGULAR_FORCE = 20;

				p->AddImpulse(FMath::RandPointInBox(FBox(FVector(-FLIP_LINEAR_FORCE, -FLIP_LINEAR_FORCE, 0), FVector(FLIP_LINEAR_FORCE, FLIP_LINEAR_FORCE, 0))), NAME_None, true);
				p->AddAngularImpulse(FMath::RandPointInBox(FBox(FVector(-FLIP_ANGULAR_FORCE, -FLIP_ANGULAR_FORCE, -FLIP_ANGULAR_FORCE), FVector(FLIP_ANGULAR_FORCE, FLIP_ANGULAR_FORCE, FLIP_ANGULAR_FORCE))), NAME_None, true);
			}
		}
		else
		{
			Destroy();
		}
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

void ABasePart::NotifyHit(UPrimitiveComponent * MyComp, AActor * Other, UPrimitiveComponent * OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	OnHitHandler(Other, OtherComp, NormalImpulse, Hit);
}

void ABasePart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePart, Health);
	DOREPLIFETIME(ABasePart, GridX);
	DOREPLIFETIME(ABasePart, GridY);
	DOREPLIFETIME(ABasePart, ExpectedParentGrid);
}