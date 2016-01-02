// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "PersistentWorldPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LD34_API APersistentWorldPlayerController : public APlayerController
{
	GENERATED_BODY()

	APersistentWorldPlayerController();
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;
	
private:
	float RespawnTimer;

	UPROPERTY(Category = Respawn, EditAnywhere)
	float RespawnDelay;

	UPROPERTY(Category = Respawn, EditAnywhere)
	TSubclassOf<class ARobotGridGenerator> ShipGenerator;

	FVector FindClearPoint();
};
