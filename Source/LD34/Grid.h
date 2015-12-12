// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Grid.generated.h"

UCLASS()
class LD34_API AGrid : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGrid();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UFUNCTION(BlueprintCallable, Category = Grid)
	void AddToGrid(class ABasePart* Part, int32 X, int32 Y);
	
	UFUNCTION(BlueprintCallable, Category = Grid)
	void CreateAndAddToGrid(TSubclassOf<class ABasePart> Part, int32 X, int32 Y);

	float ForwardBackwardThrust;
	float LeftRightThrust;
	float LeftRightTurn;

	void SetForwardBackwardThrust(float val);
	void SetLeftRightThrust(float val);
	void SetLeftRightTurn(float val);

	void SetIsFiringGroup0(float val);

	UPROPERTY(BlueprintReadWrite, Category = Destination)
	bool DestinationMode;

	UPROPERTY(BlueprintReadWrite, Category = Destination)
	FVector2D Destination;

	bool IsFiringGroup0;
private:
	TMap<int32, TMap<int32, class ABasePart*>> Cells;
};
