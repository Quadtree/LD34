// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "AsteroidGridGenerator.h"

void AAsteroidGridGenerator::DoGenerate()
{
	int32 asteroidSize = FMath::RandRange(12, 40);

	SetCell(10, 10, PartType);

	int32 actualSize = 1;

	while (actualSize < asteroidSize)
	{
		FIntPoint pt = GetCellOfType(PartType);

		if (FMath::Rand() % 2 == 0)
			pt.X += FMath::Rand() % 2 * 2 - 1;
		else
			pt.Y += FMath::Rand() % 2 * 2 - 1;

		if (SetCell(pt.X, pt.Y, PartType)) actualSize++;
	}

	UE_LOG(LogTemp, Display, TEXT("Asteroid generated, parts %s"), *FString::FromInt(actualSize));

	CreateFinalGrid(false);
}


