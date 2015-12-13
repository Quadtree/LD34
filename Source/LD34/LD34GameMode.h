// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "LD34GameMode.generated.h"

/**
 * 
 */
UCLASS()
class LD34_API ALD34GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ALD34GameMode();

	// charges up. randomly a wave will be spawned consuming this
	float WavePower;

	float WaveChargeRate;

	float WaveTimeCharge;

	int32 Difficulty;

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = Diff)
	TArray<float> InitialChargeRate;

	UPROPERTY(EditAnywhere, Category = Diff)
	TArray<float> ChargeRateRiseRate;
};
