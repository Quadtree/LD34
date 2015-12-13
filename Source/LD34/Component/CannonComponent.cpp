// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "CannonComponent.h"
#include "Grid.h"
#include "Projectile.h"


// Sets default values for this component's properties
UCannonComponent::UCannonComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCannonComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCannonComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	CooldownLeft -= DeltaTime;

	AGrid* GridParent = Cast<AGrid>(this->GetAttachmentRootActor());

	if (GridParent && (GridParent->IsFiringGroup0 && FireGroup == 0) && CooldownLeft <= 0 && GridParent->Power >= EnergyToFire)
	{
		FActorSpawnParameters p;

		p.Instigator = GridParent;

		auto a = GetOwner()->GetWorld()->SpawnActor<AProjectile>(ShotType, GetComponentLocation(), GetComponentRotation(), p);

		if (a)
		{
			a->Vel = GetComponentRotation().RotateVector(FVector(ProjectileSpeed, 0, 0));

			/*auto root = Cast<UPrimitiveComponent>(a->GetRootComponent());

			if (root)
			{
				FVector nv = GetComponentRotation().RotateVector(FVector(ProjectileSpeed, 0, 0));

				UE_LOG(LogTemp, Display, TEXT("VEL %s"), *nv.ToString());

				root->SetPhysicsLinearVelocity(nv);
			}*/

			CooldownLeft = ShotCooldown;

			UE_LOG(LogTemp, Display, TEXT("Spawned!"));

			GridParent->Power -= EnergyToFire;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Shot spawn failed!"));
		}
	}
}

