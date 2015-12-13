// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "LD34GameMode.h"

ALD34GameMode::ALD34GameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALD34GameMode::BeginPlay()
{
	// difficulty levels:
	// 0 - trivial
	// 1 - easy
	// 2 - normal
	// 3 - hard
	// 4 - expert
	// 5 - impossible

	FString diff = GetWorld()->URL.GetOption(TEXT("diff"), TEXT("2"));
	diff = diff.Replace(TEXT("="), TEXT(""));

	Difficulty = FMath::Clamp(FCString::Atoi(*diff), 0, FMath::Min(InitialChargeRate.Num() - 1, ChargeRateRiseRate.Num() - 1));

	WaveChargeRate = InitialChargeRate[Difficulty];

	WaveTimeCharge = 30;
	WavePower = WaveChargeRate * 15;
}

void ALD34GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	WaveChargeRate += ChargeRateRiseRate[Difficulty] * DeltaSeconds;
	WavePower += WaveChargeRate * DeltaSeconds;
	WaveTimeCharge += DeltaSeconds;

	if (WaveTimeCharge > 30.f)
	{
		UE_LOG(LogTemp, Display, TEXT("Spawning with power %s"), *FString::SanitizeFloat(WavePower));

		WavePower = 0;
		WaveTimeCharge = 0;
	}
}


