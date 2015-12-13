// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RobotGridGenerator.generated.h"

UCLASS()
class LD34_API ARobotGridGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARobotGridGenerator();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	TArray<TArray<TSubclassOf<class ABasePart>>> HalfGrid;

	UPROPERTY(EditAnywhere, Category = Generation)
	TArray<float> PartCost;
	
	UPROPERTY(EditAnywhere, Category = Generation)
	TArray<TSubclassOf<class ABasePart>> Part;

	UPROPERTY(EditAnywhere, Category = Generation)
	TArray<int32> PartMountType;

	UPROPERTY(EditAnywhere, Category = Generation)
	TSubclassOf<class AGrid> GridType;

	float CalcActualValue();

	UPROPERTY(EditAnywhere, Category = Generation)
	float Value;

	bool SetCell(int32 x, int32 y, TSubclassOf<class ABasePart> val);
	TSubclassOf<class ABasePart> GetCell(int32 x, int32 y);

	FIntPoint GetCellOfType(TSubclassOf<class ABasePart> type);
	FIntPoint GetCellNotOfType(TSubclassOf<class ABasePart> type);
};
