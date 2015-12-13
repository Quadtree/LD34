// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RobotGridGenerator.h"
#include "AsteroidGridGenerator.generated.h"

/**
 * 
 */
UCLASS()
class LD34_API AAsteroidGridGenerator : public ARobotGridGenerator
{
	GENERATED_BODY()
	
protected:
	virtual void DoGenerate() override;
	
	UPROPERTY(EditAnywhere, Category = Generation)
	TSubclassOf<class ABasePart> PartType;
};
