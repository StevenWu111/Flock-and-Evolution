// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsteroidCluster.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Boid.generated.h"

class ASpawner;

UCLASS()
class A3BASECODE_API ABoid : public AActor
{
	GENERATED_BODY()
	
public:
	//Constructor
	ABoid();

protected:
	//Called when game start
	virtual void BeginPlay() override;

public:
	//Called every single frame
	virtual void Tick(float DeltaTime) override;
	//few UPROPERTY to be able set the mesh,collision and perception sensor in Unreal Editor
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* BoidMesh;
	UPROPERTY(EditAnywhere)
		USphereComponent* BoidCollision;
	UPROPERTY(EditAnywhere)
		USphereComponent* PerceptionSensor;
	//How many resources we have collected
	UPROPERTY(VisibleAnywhere)
		float ResourcesCollected = 0.0f;
	//The velocity of current boid
	FVector BoidVelocity;
	//The rotation of current boid
	FRotator CurrentRotation;
	//Control the updating of the Mesh rotaion
	void UpdateMeshRotation();
	//The vector of avoid collision with other boids
	virtual FVector AvoidBoids(TArray<AActor*> Flock);
	//Control Velocity Matching vector, Harvester ship will only affect by other Harvester ships and Pirate will only affect by other Pirate
	virtual FVector VelocityMatching(TArray<AActor*> Flock);
	//The vector which pointing at the centroid of the boids who inside of perception sensor
	virtual FVector FlockCentering(TArray<AActor*> Flock);
	//The vector avoid the obstacle
	virtual FVector AvoidObstacle();
	//control the next position of the current boid
	virtual void FlightPath(float DeltaTime);
	//check is there any obstacle in front of the boid
	bool IsObstacleAhead();
	
	TArray<FVector> AsteroidForces;
	//Max and min speed for the boid
	float MinSpeed = 300.0f;
	float MaxSpeed = 600.0f;

	//Strength variables which will be altered in the evolutionary algorithm
	float VelocityMatchingStrength = 100.0f;
	float AvoidBoidsStrength = 100.0f;
	float CenteringStrength = 1.0f;
	float ObstacleAvoidanceStrength = 10000.0f;
	float AsteroidStrength = 1.0f;
	float Speed = 0;

	//How long is the invincible period for each boid, during this period, we ignore all the collisions to avoid destroyed it instantly
	float Invincibility = 1.0f;

	
	TArray<FVector> AvoidanceSensors;
	float SensorRadius = 600.0f;
	int NumSensors = 100;

	float GoldenRatio = (1.0f + FMath::Sqrt(5.0f))/2;
	//A reference to the Asteroid Cluster that we currently colliding with
	AAsteroidCluster* CollisionAsteroids;
	//A reference to the Spawner class
	ASpawner* Spawner;

	//Called when the overlap begin
	UFUNCTION()
		virtual void OnHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent,  AActor* OtherActor,  UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,  bool bFromSweep, const FHitResult& SweepResult);
	//Called when the overlap end
	UFUNCTION()
		virtual void OnHitboxOverlapEnd(UPrimitiveComponent* OverlappedComponent,  AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);
	//contain the normal vector when ship facing toward obstacle, after calculation in AvoidObstacle function, it will hold the avoidance vector
	FVector ObstacleAvoidance;
	//the max and min strength
	float MinStrength = 1.0f;
	float MaxStrength = 10000.0f;
	//the surviving time and the score
	float SurvivingTime = 0.0f;
	float FitnessScore = 0.0f;
};
