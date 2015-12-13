// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "GridAIController.generated.h"

/**
 * 
 */
UCLASS()
class LD34_API AGridAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AGridAIController();
	
	virtual void Tick(float DeltaSeconds) override;
};
