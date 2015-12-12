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
	UPROPERTY(EditAnywhere, Category = Health)
	int32 Health;
	
};
