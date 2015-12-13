// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "SpacePlayerController.h"
#include "Grid.h"
#include "EngineUtils.h"

ASpacePlayerController::ASpacePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpacePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	

	auto g = Cast<AGrid>(GetPawn());

	if (g) { UE_LOG(LogTemp, Display, TEXT("TICK! %s"), *FString::FromInt(g->CommandCenters)); }

	if (!g || !g->CommandCenters)
	{
		int32 n = 0;
		AGrid* jumpTarget = nullptr;

		for (TActorIterator<AGrid> i(GetWorld()); i; ++i)
		{
			if (i->Faction == 0 && i->CommandCenters && ((FMath::Rand() % ++n) == 0))
			{
				jumpTarget = *i;
			}
		}

		if (jumpTarget)
		{
			this->Possess(jumpTarget);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Player loses."));
			OnLose();
		}
	}
}


