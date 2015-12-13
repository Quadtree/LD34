// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "LD34GameMode.h"
#include "RobotGridGenerator.h"
#include "Grid.h"
#include "Part/BasePart.h"
#include "AsteroidGridGenerator.h"

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

	WaveTimeCharge = 60;
	WavePower = FMath::Max(WaveChargeRate * 15, 12.f);

	for (int32 i = 0; i < 20; ++i)
	{
		GetWorld()->SpawnActor<ARobotGridGenerator>(AsteroidGeneratorType, FindClearPoint(), FRotator(0, FMath::FRandRange(0, 360), 0));
	}
}

void ALD34GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	WaveChargeRate += ChargeRateRiseRate[Difficulty] * DeltaSeconds * 2.f;
	WavePower += WaveChargeRate * DeltaSeconds * 0.3f;
	WaveTimeCharge += DeltaSeconds;

	if (WaveTimeCharge > 45.f)
	{
		UE_LOG(LogTemp, Display, TEXT("Spawning with power %s"), *FString::SanitizeFloat(WavePower));

		while (WavePower > 15)
		{
			float nextValue = FMath::FRandRange(FMath::Max(WavePower / 2, 15.f), WavePower);

			auto a = GetWorld()->SpawnActor<ARobotGridGenerator>(GeneratorType, FindClearPoint(), FRotator::ZeroRotator);

			if (a)
			{
				a->Value = nextValue;
				a->Faction = 1;
			}

			WavePower -= nextValue;

			WaveTimeCharge = 0;
		}

		
	}
}

FVector ALD34GameMode::FindClearPoint()
{
	FVector ret;

	for (int32 i = 0; i < 10000; ++i)
	{
		ret = FMath::RandPointInBox(FBox(FVector(-20000, -20000, 0), FVector(20000, 20000, 0)));

		TArray<FOverlapResult> res;

		bool hitAGrid = false;

		if (GetWorld()->OverlapMultiByChannel(res, ret, FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeSphere(1000)))
		{
			for (auto a : res)
			{
				if (a.Actor.Get() && (Cast<AGrid>(a.Actor.Get()) || Cast<ABasePart>(a.Actor.Get())))
				{
					hitAGrid = true;
					break;
				}
			}
		}

		if (!hitAGrid)
		{
			break;
		}
	}

	return ret;
}


