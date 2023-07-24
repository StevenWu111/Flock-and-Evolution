// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Harvester.h"
#include "Boid.h"
#include "Pirate.generated.h"

/**
 * 
 */
UCLASS()
class A3BASECODE_API APirate : public ABoid
{
	GENERATED_BODY()

public:
	//Constructor
	APirate();
	//4 forces that we overrided
	virtual FVector AvoidBoids(TArray<AActor*> Flock) override;
	virtual FVector VelocityMatching(TArray<AActor*> Flock) override;
	virtual FVector FlockCentering(TArray<AActor*> Flock) override;
	virtual FVector AvoidObstacle() override;
	//tick and plight path
	virtual void Tick(float DeltaSeconds) override;
	virtual void FlightPath(float DeltaTime) override;
	//use to calculate the vector toward the harvester ship
	FVector TowardHarv();
	//a reference of harvester ship and initialize it with nullptr
	AActor* HarvesterShip = nullptr;
	//called when overlap begin
	virtual void OnHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	//current ship data
	TArray<float> ShipData;
	//the remain time of keep freeze
	float StayTime = 0;
};
