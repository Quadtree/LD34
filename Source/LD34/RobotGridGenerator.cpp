// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "RobotGridGenerator.h"
#include "Part/BasePart.h"
#include "Grid.h"

const int32 PART_ID_ARMOR = 0;
const int32 PART_ID_COMMAND = 1;
const int32 PART_ID_BOLT_CANNON = 2;
const int32 PART_ID_FUSION_REACTOR = 3;
const int32 PART_ID_THRUSTER = 4;
const int32 PART_ID_ENGINE = 5;
const int32 PART_ID_BOLT_TURRET = 6;
const int32 PART_ID_PLASMA_CANNON = 7;
const int32 PART_ID_SHIELD_GENERATOR = 8;
const int32 PART_ID_ARMOR2 = 9;

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
	
	
}

// Called every frame
void ARobotGridGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (SpawnDone) return;

	for (int32 x = 0; x < 25; ++x)
	{
		HalfGrid.Add(TArray<TSubclassOf<class ABasePart>>());

		for (int32 y = 0; y < 25; ++y)
		{
			HalfGrid[x].Add(TSubclassOf<class ABasePart>());
		}
	}

	DoGenerate();
}

float ARobotGridGenerator::CalcActualValue(bool validate)
{
	float ret = 0;

	commandCenters = 0;
	engines = 0;
	weapons = 0;
	power = 0;

	for (int32 x = 0; x < 20; ++x)
	{
		for (int32 y = 0; y < 20; ++y)
		{
			int32 typeIndex = -1;
			auto inCell = GetCell(x, y);

			for (int32 i = 0; i < Part.Num(); ++i)
			{
				if (Part[i] == inCell)
				{
					typeIndex = i;
					break;
				}
			}

			if (typeIndex == 1) commandCenters += (y == 0 ? 1 : 2);
			if (typeIndex == 4 || typeIndex == 5) engines += (y == 0 ? 1 : 2);
			if (typeIndex == 2 || typeIndex == 6 || typeIndex == 7) weapons += (y == 0 ? 1 : 2);
			if (typeIndex == 3) power += (y == 0 ? 1 : 2);

			if (typeIndex != -1)
			{
				ret += PartCost[typeIndex] * (y == 0 ? 1 : 2);
			}
		}
	}

	if (validate && (!commandCenters || (engines < Value / 3) || !weapons || !power)) return 0;

	return ret;
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
	if (x < 0 || y < 0 || x >= 20 || y >= 20) return TSubclassOf<class ABasePart>();

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
			if (HalfGrid[i][j] && HalfGrid[i][j] != type && FMath::Rand() % ++n == 0) ret = FIntPoint(i, j);
		}
	}

	return ret;
}

void ARobotGridGenerator::DoGenerate()
{
	int32 targetArmorBlocks = FMath::FloorToInt(FMath::FRandRange(Value * 0.666f, Value * 1.f));

	SetCell(10, 0, Part[0]);

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
				if (SetCell(pt.X, pt.Y, Part[0])) armorBlocks += (pt.Y == 0 ? 1 : 2);

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
					if (SetCell(x, y, Part[0])) armorBlocks += (y == 0 ? 1 : 2);
				}
			}
		}
	}

	int32 itr = 0;

	const int32 THAT_RATIO = 14;

	while (true)
	{
		if (++itr > 10000)
		{
			UE_LOG(LogTemp, Warning, TEXT("Iteration limit reached"));
			break;
		}

		float actVal = CalcActualValue(false);

		if (power >= Value / THAT_RATIO && engines >= Value / THAT_RATIO && weapons >= Value / THAT_RATIO && commandCenters && actVal > Value) break;

		//UE_LOG(LogTemp, Display, TEXT("%s vs %s"), *FString::SanitizeFloat(actVal), *FString::SanitizeFloat(Value));

		/*if (actVal > Value)
		{
		FIntPoint pt = GetCellNotOfType(Part[0]);

		SetCell(pt.X, pt.Y, Part[0]);
		}
		else
		{*/
		int32 pInd = FMath::RandRange(1, Part.Num() - 1);


		if (power < Value / THAT_RATIO) pInd = PART_ID_FUSION_REACTOR;
		if (engines < Value / THAT_RATIO) pInd = PART_ID_THRUSTER;
		if (weapons < Value / THAT_RATIO) pInd = (FMath::Rand() % 2) ? PART_ID_BOLT_CANNON : (FMath::Rand() % 2 == 0 ? PART_ID_BOLT_TURRET : PART_ID_PLASMA_CANNON);

		if (!commandCenters) pInd = PART_ID_COMMAND;

		FIntPoint pt;

		for (int32 i = 0; i < 20; ++i)
		{
			pt = GetCellOfType(Part[0]);
			break;

			if (PartMountType[pInd] == 0)
			{
				break;
			}
			else if (PartMountType[pInd] == 1)
			{
				// front mount
				if (!GetCell(pt.X + 1, pt.Y))
				{
					break;
				}
			}
			else if (PartMountType[pInd] == -1)
			{
				// rear mount
				if (!GetCell(pt.X - 1, pt.Y))
				{
					break;
				}
			}
		}

		//UE_LOG(LogTemp, Display, TEXT("Placing %s at %s"), *FString::FromInt(pInd), *pt.ToString());

		SetCell(pt.X, pt.Y, Part[pInd]);
		//}
	}

	CreateFinalGrid(true);
}

void ARobotGridGenerator::CreateFinalGrid(bool symmetry)
{
	AGrid* g = GetWorld()->SpawnActor<AGrid>(GridType, GetActorLocation(), GetActorRotation());

	if (g)
	{
		for (int32 x = 0; x < 20; ++x)
		{
			for (int32 y = 0; y < 20; ++y)
			{
				if (HalfGrid[x][y])
				{
					g->CreateAndAddToGrid(HalfGrid[x][y], x - 10, y);

					if (y != 0 && symmetry) g->CreateAndAddToGrid(HalfGrid[x][y], x - 10, -y);
				}
			}
		}

		g->Faction = Faction;
		g->SpawnDefaultController();

		Destroy();

		SpawnDone = true;
	}
}

