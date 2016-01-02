// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "PersistentWorldPlayerController.h"
#include "Grid.h"
#include "RobotGridGenerator.h"
#include "Part/BasePart.h"

APersistentWorldPlayerController::APersistentWorldPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	RespawnDelay = 5;
	RespawnTimer = RespawnDelay;
}

void APersistentWorldPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (this->Role == ENetRole::ROLE_Authority)
	{
		auto g = Cast<AGrid>(GetPawn());
	
		if (g)
		{
			if (g->CommandCenters <= 0)
			{
				RespawnTimer -= DeltaSeconds;
			}
			else
			{
				RespawnTimer = RespawnDelay;
			}
		}
		else
		{
			RespawnTimer -= DeltaSeconds;
		}

		UE_LOG(LogTemp, Display, TEXT("Respawn time %s"), *FString::SanitizeFloat(RespawnTimer));

		if (RespawnTimer <= 0)
		{
			RespawnTimer = RespawnDelay;

			auto a = GetWorld()->SpawnActor<ARobotGridGenerator>(ShipGenerator, FindClearPoint(), FRotator::ZeroRotator);
			
			if (a)
			{
				a->FutureController = this;
			}
		}
	}
}

void APersistentWorldPlayerController::BeginPlay()
{
	RespawnTimer = RespawnDelay;
}

FVector APersistentWorldPlayerController::FindClearPoint()
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