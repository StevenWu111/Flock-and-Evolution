// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boid.h"
#include "../../../../VS/VC/Tools/MSVC/14.34.31933/INCLUDE/stdbool.h"
#include "Harvester.generated.h"

/**
 * 
 */
UCLASS()
class A3BASECODE_API AHarvester : public ABoid
{
	GENERATED_BODY()

public:
	AHarvester();

	//4 override function from boid parent class, used to calculate the forces
	virtual FVector AvoidBoids(TArray<AActor*> Flock) override;
	virtual FVector VelocityMatching(TArray<AActor*> Flock) override;
	virtual FVector FlockCentering(TArray<AActor*> Flock) override;
	virtual FVector AvoidObstacle() override;
	//Additional forces to calculate the vector that toward the asteroid
	FVector TowardAsteroid();
	//represent whether overlap with asteroid or not
	bool HitAsteroid = false;
	//Use this to check we got Asteroid from spawner or not
	bool GetAsteroid = true;
	//Save the index of the current asteroid that we targeting to
	int CurrentTargetingAsteroid = 0;
	//Use to save the locations in order based on the distance between the ship and the asteroids
	TArray<FVector> LocationInOrder;
	//few override function from parent boid class
	virtual void Tick(float DeltaSeconds) override;
	virtual void FlightPath(float DeltaTime) override;
	virtual void OnHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnHitboxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex) override;
	//Use this to save the ShipData
	TArray<float> ShipData;
};