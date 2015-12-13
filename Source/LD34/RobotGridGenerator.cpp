// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "RobotGridGenerator.h"
#include "Part/BasePart.h"
#include "Grid.h"


// Sets default values
ARobotGridGenerator::ARobotGridGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARobotGridGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	for (int32 x = 0; x < 20; ++x)
	{
		HalfGrid.Add(TArray<TSubclassOf<class ABasePart>>());
		
		for (int32 y = 0; y < 20; ++y)
		{
			HalfGrid[x].Add(TSubclassOf<class ABasePart>());
		}
	}

	int32 targetArmorBlocks = FMath::RandRange(Value / 4, Value);

	SetCell(0, 10, Part[0]);

	int32 armorBlocks = 1;

	while (armorBlocks < targetArmorBlocks)
	{
		if (FMath::Rand() % 2 == 0)
		{
			FIntPoint pt = GetCellOfType(Part[0]);

			FIntPoint move(0, 0);

			if (FMath::Rand() % 2 == 0)
				move.X = FMath::RandRange(-1, 1);
			else
				move.Y = FMath::RandRange(-1, 1);

			while (FMath::Rand() % 4 != 0 && armorBlocks < targetArmorBlocks)
			{
				if (SetCell(pt.X, pt.Y, Part[0])) armorBlocks += pt.X == 0 ? 1 : 2;

				pt += move;
			}
		}
		else
		{
			FIntPoint pt = GetCellOfType(Part[0]);

			FIntPoint pt2 = pt + FIntPoint(FMath::RandRange(0, 2), FMath::RandRange(0, 2));

			/*int32 minX = FMath::Min(pt.X, pt2.X);
			int32 maxX = FMath::Max(pt.X, pt2.X);
			int32 minY = FMath::Min(pt.Y, pt2.Y);
			int32 maxY = FMath::Max(pt.Y, pt2.Y);*/

			for (int32 x = pt.X; x <= pt2.X; ++x)
			{
				for (int32 y = pt.Y; y <= pt2.Y; ++y)
				{
					if (SetCell(x, y, Part[0])) armorBlocks += x == 0 ? 1 : 2;
				}
			}
		}
	}

	AGrid* g = GetWorld()->SpawnActor<AGrid>(GridType, GetActorLocation(), GetActorRotation());

	if (g)
	{
		for (int32 x = 0; x < 20; ++x)
		{
			for (int32 y = 0; y < 20; ++y)
			{
				if (HalfGrid[x][y])
				{
					g->CreateAndAddToGrid(HalfGrid[x][y], x, y - 10);

					if (x != 0) g->CreateAndAddToGrid(HalfGrid[x][y], -x, y - 10);
				}
			}
		}

		Destroy();
	}
}

// Called every frame
void ARobotGridGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool ARobotGridGenerator::SetCell(int32 x, int32 y, TSubclassOf<class ABasePart> val)
{
	if (x < 0 || y < 0 || x >= 20 || y >= 20) return false;

	if (HalfGrid[x][y] == val) return false;

	HalfGrid[x][y] = val;

	return true;
}

TSubclassOf<class ABasePart> ARobotGridGenerator::GetCell(int32 x, int32 y)
{
	if (x < 0 || y < 0 || x >= 20 || y >= 20) TSubclassOf<class ABasePart>();

	return HalfGrid[x][y];
}

FIntPoint ARobotGridGenerator::GetCellOfType(TSubclassOf<class ABasePart> type)
{
	int32 n = 0;

	FIntPoint ret(-1,-1);

	for (int32 i = 0; i < 20; ++i)
	{
		for (int32 j = 0; j < 20; ++j)
		{
			if (HalfGrid[i][j] == type && FMath::Rand() % ++n == 0) ret = FIntPoint(i, j);
		}
	}

	return ret;
}

FIntPoint ARobotGridGenerator::GetCellNotOfType(TSubclassOf<class ABasePart> type)
{
	int32 n = 0;

	FIntPoint ret(-1, -1);

	for (int32 i = 0; i < 20; ++i)
	{
		for (int32 j = 0; j < 20; ++j)
		{
			if (HalfGrid[i][j] != type && FMath::Rand() % ++n == 0) ret = FIntPoint(i, j);
		}
	}

	return ret;
}

