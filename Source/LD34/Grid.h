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

	UFUNCTION(BlueprintCallable, Category = Grid, NetMulticast, Reliable)
	void AddToGrid(class ABasePart* Part, int32 X, int32 Y);
	
	UFUNCTION(BlueprintCallable, Category = Grid)
	void CreateAndAddToGrid(TSubclassOf<class ABasePart> Part, int32 X, int32 Y);

	void RemoveAt(int32 X, int32 Y);

	float ForwardBackwardThrust;
	float LeftRightThrust;
	float LeftRightTurn;

	void SetForwardBackwardThrust(float val);
	void SetLeftRightThrust(float val);
	void SetLeftRightTurn(float val);

	void SetIsFiringGroup0(float val);

	void ZoomIn();
	void ZoomOut();

	UPROPERTY(BlueprintReadWrite, Category = Destination)
	bool DestinationMode;

	UPROPERTY(BlueprintReadWrite, Category = Destination)
	FVector2D Destination;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Faction)
	int32 Faction;

	bool IsFiringGroup0;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void ContinuityCheck();
	TArray<class ABasePart*> GetAllContiguous(class ABasePart* start);
	class ABasePart* GetPartAt(int32 X, int32 Y);

	UPROPERTY(EditAnywhere, Category = Split)
	TSubclassOf<AGrid> BaseGridType;

	int32 CommandCenters;

	UFUNCTION()
	void OnHitHandler(AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit);

	float Power;

	float MaxPower;

	float PowerRegenRate;

	UFUNCTION(BlueprintPure, Category = Power)
	float GetPowerPct();

	UPROPERTY(EditAnywhere, Category = Zoom)
	float MaxCameraHeight;

	UPROPERTY(EditAnywhere, Category = Zoom)
	float MinCameraHeight;

	UPROPERTY(EditAnywhere, Category = Zoom)
	float CameraDefaultHeight;

	float CameraHeight;

	float CameraDesiredHeight;

	UPROPERTY(EditAnywhere, Category = Zoom)
	float CameraMovespeed;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* AddPartSound;

	UPROPERTY(EditAnywhere, Category = Zoom)
	float ES2Brightness;

	void RecalculateBounds();

	UPROPERTY(BlueprintReadOnly, Category = Bounding)
	float BoundingSphereRadius;

	UPROPERTY(BlueprintReadOnly, Category = Bounding)
	FVector BoundingSphereCenter;

	UPROPERTY(BlueprintReadOnly, Category = Shield)
	float Shield;

	UPROPERTY(BlueprintReadOnly, Category = Shield)
	float ShieldMax;

	UPROPERTY(BlueprintReadOnly, Category = Shield)
	float ShieldRegenRate;

	UFUNCTION(BlueprintPure, Category = Shield)
	float GetShieldPct();

private:
	TMap<int32, TMap<int32, class ABasePart*>> Cells;
};
