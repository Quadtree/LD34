// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "Projectile.h"


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
	
}

// Called every frame
void AProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	SetActorLocation(GetActorLocation() + Vel * DeltaTime);

	//UE_LOG(LogTemp, Display, TEXT("POS %s"), *GetActorLocation().ToString());

	TArray<FOverlapResult> res;

	if (GetWorld()->OverlapMultiByChannel(res, FVector(GetActorLocation().X, GetActorLocation().Y, 0), FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeSphere(10)))
	{
		for (auto& rd : res)
		{
			if (rd.Actor.Get() && rd.Actor != this->GetInstigator() && rd.Actor->GetRootComponent() && rd.Actor->GetRootComponent()->GetAttachmentRootActor() != this->GetInstigator() && rd.Component.Get() && rd.Component->GetOwner())
			{
				UE_LOG(LogTemp, Display, TEXT("Hit %s"), *rd.Component->GetName());
				rd.Component->GetOwner()->TakeDamage(DamageOnHit, FDamageEvent(), this->GetInstigatorController(), this);
				this->Destroy();
			}
		}
	}
}

