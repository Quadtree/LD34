// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "CannonComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LD34_API UCannonComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCannonComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(EditAnywhere, Category = Cannon)
	TSubclassOf<class AProjectile> ShotType;

	UPROPERTY(EditAnywhere, Category = Cannon)
	float ShotCooldown;

	UPROPERTY(EditAnywhere, Category = Cannon)
	float EnergyToFire;

	UPROPERTY(EditAnywhere, Category = Cannon)
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere, Category = Cannon)
	int32 FireGroup;

private:
	float CooldownLeft;
};
