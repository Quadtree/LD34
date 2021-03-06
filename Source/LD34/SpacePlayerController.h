// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "SpacePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LD34_API ASpacePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ASpacePlayerController();
	
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintImplementableEvent, Category = WinLoss)
	void OnLose();

	UFUNCTION(BlueprintImplementableEvent, Category = WinLoss)
	void OnWin();
};
