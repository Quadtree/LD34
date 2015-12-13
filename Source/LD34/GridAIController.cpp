// Fill out your copyright notice in the Description page of Project Settings.

#include "LD34.h"
#include "GridAIController.h"
#include "Grid.h"
#include "EngineUtils.h"

AGridAIController::AGridAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGridAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	auto p = Cast<AGrid>(GetPawn());

	if (p && p->CommandCenters)
	{
		FVector desiredVV(0,0,0);
		FVector currentVV;

		auto myRoot = Cast<UPrimitiveComponent>(GetRootComponent());
		if (myRoot)
		{
			currentVV = myRoot->GetPhysicsLinearVelocity();
		}

		AGrid* target = nullptr;

		{
			float bestDistSquared = 1000000000000000000;

			for (TActorIterator<AGrid> i(GetWorld()); i; ++i)
			{
				if (i->Faction != p->Faction && i->CommandCenters)
				{
					float distSquared = FVector::DistSquared(p->GetActorLocation(), i->GetActorLocation());

					if (distSquared < bestDistSquared)
					{
						bestDistSquared = distSquared;
						target = *i;
					}
				}
			}
		}

		if (target)
		{
			FVector trg = target->GetActorLocation();

			auto trgRoot = Cast<UPrimitiveComponent>(target->GetRootComponent());

			if (trgRoot) trg = trgRoot->GetCenterOfMass();

			//DrawDebugSphere(GetWorld(), trg, 200, 2, FColor::Red, false, 1);

			//UE_LOG(LogTemp, Display, TEXT("TARGET %s"), *FVector2D(trg).ToString());

			p->Destination = FVector2D(trg);
			p->DestinationMode = true;

			float range = FVector::Dist(p->GetActorLocation(), trg);

			FVector delta = trg - p->GetActorLocation();
			delta.Z = 0;
			delta.Normalize();

			p->IsFiringGroup0 = range < 6000;

			if (range < 1200)
			{
				desiredVV = -delta * 800;
			}
			else if (range < 3000)
			{
				desiredVV = FVector(0,0,0);
			}
			else
			{
				desiredVV = delta * 800;
			}
		}

		{
			float bestThrust = 0;
			float bestStrafe = 0;
			float bestDistSquared = 100000000000;

			for (int32 thrust = -1; thrust <= 1; ++thrust)
			{
				for (int32 strafe = -1; strafe <= 1; ++strafe)
				{
					FVector newVV = currentVV;

					newVV += p->GetActorRotation().RotateVector(FVector::ForwardVector) * thrust;
					newVV += p->GetActorRotation().RotateVector(FVector::RightVector) * strafe;

					float distSquared = FVector::DistSquared(newVV, desiredVV);

					if (distSquared < bestDistSquared)
					{
						bestDistSquared = distSquared;
						bestThrust = thrust;
						bestStrafe = strafe;
					}
				}
			}

			//UE_LOG(LogTemp, Display, TEXT("%s -- %s"), *FString::SanitizeFloat(bestThrust), *FString::SanitizeFloat(bestStrafe));

			p->ForwardBackwardThrust = bestThrust;
			p->LeftRightThrust = bestStrafe;
		}
	}
}
