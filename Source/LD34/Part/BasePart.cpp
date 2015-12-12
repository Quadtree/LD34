// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "BasePart.h"


// Sets default values
ABasePart::ABasePart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABasePart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasePart::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

