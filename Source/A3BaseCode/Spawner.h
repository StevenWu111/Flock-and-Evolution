// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boid.h"
#include "Pirate.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UCLASS()
class A3BASECODE_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawner();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	//Max amount of both ships and asteroids
	UPROPERTY(EditAnywhere, Category = "Entities")
		float MaxHarvesterShipCount = 300;
	UPROPERTY(EditAnywhere, Category = "Entities")
		float MaxPirateShipCount = 20;
	UPROPERTY(EditAnywhere, Category = "Entities")
		float MaxAsteroidCount = 5;
	
	//References of both ships and asteroids
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<ABoid> HarvesterShipClass;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<ABoid> PirateShipClass;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AAsteroidCluster> AsteroidClusterClass;

	//Current amount of both ships
	int NumHarvesterShips = 0;
	int NumPirateShips = 0;
	//An Array for the AsteroidCluster
	TArray<AAsteroidCluster*> AsteroidClusters;

	//Spawn both ships
	AHarvester* SpawnHarvesterShip();
	APirate* SpawnPirateShip();
	//Set the variable of it
	void SetShipVariables(ABoid* Ship);
	//A overload function to allow a input of data which is a float Array
	void SetShipVariables(ABoid* Ship, TArray<float> Data);	

	//Use to store the population of both ships, will be save in here before they destroy themselves
	TArray<AHarvester*> Harvesters;
	TArray<APirate*> Pirates;

	//Generate both ships based on evaluation algorithm
	void HarvesterGeneration();
	void PirateGeneration();
	//Breed 200 time for each time we generate offspring (Harvester)
	int NumBreedings = 200;
	//14 times for Pirate
	int NumPirateBreedings = 14;
	//the mutation chance
	float MutationChance = 0.1;
	//use this to mutate the data that we calculated
	TArray<float> Mutation(TArray<float> Data);
	//a variable to save the highest score for both ships
	float HighestHarvesterScore = 0;
	float HighestPirateScore = 0;
	//record the generations
	int HarvesterGenerationNum = 0;
	int PriateGenerationNum = 0;
	//the index of the ship has the highest score
	int HHighestIndex = 0;
	int PHighestIndex = 0;
};


