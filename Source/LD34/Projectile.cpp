// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "Projectile.h"
#include "Grid.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	Lifespan = 2;
}

// Called every frame
void AProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	SetActorLocation(GetActorLocation() + Vel * DeltaTime);

	//UE_LOG(LogTemp, Display, TEXT("POS %s"), *GetActorLocation().ToString());

	TArray<FOverlapResult> res;

	if (GetWorld()->OverlapMultiByChannel(res, FVector(GetActorLocation().X, GetActorLocation().Y, 0), FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeSphere(40)))
	{
		for (auto& rd : res)
		{
			if (rd.Actor.Get() && rd.Actor != this->GetInstigator() && rd.Actor->GetRootComponent() && rd.Actor->GetRootComponent()->GetAttachmentRootActor() != this->GetInstigator() && Cast<AGrid>(rd.Actor->GetRootComponent()->GetAttachmentRootActor()) && rd.Component.Get() && rd.Component->GetOwner())
			{
				for (float dist = 0; dist < 120; dist += 5)
				{
					FPointDamageEvent pt;
					pt.Damage = DamageOnHit;
					pt.HitInfo.ImpactPoint = GetActorLocation() + FMath::RandPointInBox(FBox(FVector(-dist, -dist, 0), FVector(dist, dist, 0)));

					//UE_LOG(LogTemp, Display, TEXT("Hit %s damage to %s %s"), *rd.Component->GetName(), *rd.Component->GetOwner()->GetName(), *FString::FromInt(pt.ClassID));

					float dmg = rd.Component->GetOwner()->TakeDamage(DamageOnHit, pt, this->GetInstigatorController(), this);

					if (dmg > 0.1f)
					{
						this->Destroy();
						return;
					}
				}
			}
		}
	}

	if ((Lifespan -= DeltaTime) <= 0) Destroy();
}

