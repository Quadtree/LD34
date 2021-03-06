// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BasePart.generated.h"

UCLASS()
class LD34_API ABasePart : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePart();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// a typical bolt does 1000 health worth of damage
	// if a part is reduced to zero health it goes flying, or if unattached, is destroyed
	UPROPERTY(EditAnywhere, Category = Health, Replicated)
	float Health;

	float MaxHealth;
	
	UPROPERTY(Replicated)
	int32 GridX;

	UPROPERTY(Replicated)
	int32 GridY;

	UPROPERTY(Replicated)
	class AGrid* ExpectedParentGrid;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	FString LockedGrid;

	float GridLockTime;

	UFUNCTION(NetMulticast, Reliable)
	void DetachFromGrid();

	UPROPERTY(EditAnywhere, Category = PartType)
	bool IsCommandCenter;

	UPROPERTY(EditAnywhere, Category = PartType)
	float OddsOfSurvival;

	UPROPERTY(EditAnywhere, Category = Power)
	float MaxPowerMod;

	UPROPERTY(EditAnywhere, Category = Power)
	float PowerRegenMod;

	UPROPERTY(EditAnywhere, Category = Shield)
	float ShieldMaxMod;

	UPROPERTY(EditAnywhere, Category = Shield)
	float ShieldRegenMod;

	UFUNCTION(NetMulticast, Reliable)
	void GoFlipping();

	UFUNCTION()
	void OnHitHandler(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* ExplosionSound;

	float LooseTime;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
};
