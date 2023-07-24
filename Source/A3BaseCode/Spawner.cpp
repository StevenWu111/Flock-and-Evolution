// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"

// Sets default values
ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	//spawn up to the maximum amounts of both ships as well as AsteroidClusters
	for(int i = 0; i < MaxHarvesterShipCount; i++)
	{
		SpawnHarvesterShip();
	}
	for(int i = 0; i < MaxPirateShipCount; i++)
	{
		SpawnPirateShip();
	}
	for(int i = 0; i < MaxAsteroidCount; i++)
	{
		//Spawn randomly and add into AsteroidClusters Array
		FVector SpawnLocation(FMath::RandRange(0.0f, 5000.0f),FMath::RandRange(-2500.0f, 2500.0f),FMath::RandRange(-2500.0f, 2500.0f));
		if(AsteroidClusterClass)
		{
			AsteroidClusters.Add(Cast<AAsteroidCluster>(GetWorld()->SpawnActor(AsteroidClusterClass, &SpawnLocation)));
		}
	}
	
}

void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Spawn both ships to the max amount when their number less than 20% of Max count
	if(NumHarvesterShips < MaxHarvesterShipCount * 0.2f)
	{
		TArray<AHarvester*> TempHarvesters;
		if (!Harvesters.IsEmpty())
		{
			TempHarvesters = Harvesters;
		}
		//Generate offsprings
		HarvesterGeneration();
		//fill the rest with random ship
		while(NumHarvesterShips < MaxHarvesterShipCount)
		{
			SpawnHarvesterShip();
		}
		//keep the current number of generation
		HarvesterGenerationNum++;
		//Display the data on screen
		GEngine->AddOnScreenDebugMessage(1, 10, FColor::White, FString::Printf(TEXT("Highest Harvester Fitness: %f"), HighestHarvesterScore));
		GEngine->AddOnScreenDebugMessage(2, 10, FColor::White, FString::Printf(TEXT("Current Harvester Generation: %d"), HarvesterGenerationNum));
		GEngine->AddOnScreenDebugMessage(3, 10, FColor::White, FString::Printf(TEXT("Current Harvester VelocityMaching strength: %f"), TempHarvesters[HHighestIndex]->VelocityMatchingStrength));
		GEngine->AddOnScreenDebugMessage(4, 10, FColor::White, FString::Printf(TEXT("Current Harvester AvoidBoid strength: %f"), TempHarvesters[HHighestIndex]->AvoidBoidsStrength));
		GEngine->AddOnScreenDebugMessage(5, 10, FColor::White, FString::Printf(TEXT("Current Harvester Centering strength: %f"), TempHarvesters[HHighestIndex]->CenteringStrength));
		GEngine->AddOnScreenDebugMessage(6, 10, FColor::White, FString::Printf(TEXT("Current Harvester ObstacleAvoidance strength: %f"), TempHarvesters[HHighestIndex]->ObstacleAvoidanceStrength));
		GEngine->AddOnScreenDebugMessage(7, 10, FColor::White, FString::Printf(TEXT("Current Harvester Asteroid strength: %f"), TempHarvesters[HHighestIndex]->AsteroidStrength));
		GEngine->AddOnScreenDebugMessage(8, 10, FColor::White, FString::Printf(TEXT("Current Harvester Speed: %f"), TempHarvesters[HHighestIndex]->Speed));
		//clear the population data
		Harvesters.Empty();
	}
	if(NumPirateShips < MaxPirateShipCount * 0.2f && !Pirates.IsEmpty())
	{
		TArray<APirate*> TempPirate;
		if (!Pirates.IsEmpty())
		{
			TempPirate = Pirates;
		}
		//Generate offsprings
		PirateGeneration();
		//fill the rest with random ship
		while(NumPirateShips < MaxPirateShipCount)
		{
			SpawnPirateShip();
		}
		//keep the current number of generation
		PriateGenerationNum++;
		//Display the data on screen
		GEngine->AddOnScreenDebugMessage(9, 10, FColor::Red, FString::Printf(TEXT("Highest Pirate Fitness: %f"), HighestPirateScore));
		GEngine->AddOnScreenDebugMessage(10, 10, FColor::Red, FString::Printf(TEXT("Current Pirate Generation: %d"), PriateGenerationNum));
		GEngine->AddOnScreenDebugMessage(11, 10, FColor::Red, FString::Printf(TEXT("Current Pirate VelocityMaching strength: %f"), TempPirate[PHighestIndex]->VelocityMatchingStrength));
		GEngine->AddOnScreenDebugMessage(12, 10, FColor::Red, FString::Printf(TEXT("Current Pirate AvoidBoid strength: %f"), TempPirate[PHighestIndex]->AvoidBoidsStrength));
		GEngine->AddOnScreenDebugMessage(13, 10, FColor::Red, FString::Printf(TEXT("Current Pirate Centering strength: %f"), TempPirate[PHighestIndex]->CenteringStrength));
		GEngine->AddOnScreenDebugMessage(14, 10, FColor::Red, FString::Printf(TEXT("Current Pirate ObstacleAvoidance strength: %f"), TempPirate[PHighestIndex]->ObstacleAvoidanceStrength));
		GEngine->AddOnScreenDebugMessage(15, 10, FColor::Red, FString::Printf(TEXT("Current Pirate Asteroid strength: %f"), TempPirate[PHighestIndex]->AsteroidStrength));
		GEngine->AddOnScreenDebugMessage(16, 10, FColor::Red, FString::Printf(TEXT("Current Pirate Speed: %f"), TempPirate[PHighestIndex]->Speed));
		//clear the population data
		Pirates.Empty();
	}

}

//Spawn both ship randomly inside of cube
AHarvester* ASpawner::SpawnHarvesterShip()
{
	//Get the random location inside of cube
	FVector SpawnLocation(FMath::RandRange(500.0f, 4500.0f),FMath::RandRange(-2000.0f, 2000.0f),FMath::RandRange(-2000.0f, 2000.0f));
	//If there is a Harvester ship class exist (setup in unreal blueprint)
	if(HarvesterShipClass)
	{
		//spawn the ship can cast to the boid class which is the parent class of it
		AHarvester* SpawnedShip = Cast<AHarvester>(GetWorld()->SpawnActor(HarvesterShipClass, &SpawnLocation,&FRotator::ZeroRotator));
		//if we spawn and cast successfully
		if(SpawnedShip)
		{
			//Set the ship's spawner to this
			SpawnedShip->Spawner = this;
			//set the variables of the ships
			//SetShipVariables(SpawnedShip);
			//Update the number of Harvester ships
			NumHarvesterShips++;
			return SpawnedShip;
		}
	}
	return nullptr;
}
//spawn the pirate ship by using the same way as the Harvester ships
APirate* ASpawner::SpawnPirateShip()
{
	FVector SpawnLocation(FMath::RandRange(500.0f, 4500.0f),FMath::RandRange(-2000.0f, 2000.0f),FMath::RandRange(-2000.0f, 2000.0f));
	if(PirateShipClass)
	{
		APirate* SpawnedShip = Cast<APirate>(GetWorld()->SpawnActor(PirateShipClass, &SpawnLocation,&FRotator::ZeroRotator));
		if(SpawnedShip)
		{
			SpawnedShip->Spawner = this;
			//SetShipVariables(SpawnedShip);
			NumPirateShips++;
			return SpawnedShip;
		}
	}
	return nullptr;
}

void ASpawner::SetShipVariables(ABoid* Ship)
{
	
}

//set the ships based on the data that we put in
void ASpawner::SetShipVariables(ABoid* Ship, TArray<float> Data)
{
	//if it is a harvester ship
	if (Data.Num() == 6 && Cast<AHarvester>(Ship))
	{
		//update each attribute based on data
		Ship->VelocityMatchingStrength = Data[0];
		Ship->AvoidBoidsStrength = Data[1];
		Ship->CenteringStrength = Data[2];
		Ship->ObstacleAvoidanceStrength = Data[3];
		Ship->AsteroidStrength = Data[4];
		Ship->Speed = Data[5];
	}
	else
	{
		//if it is a pirate ship, first 5 is same
		Ship->VelocityMatchingStrength = Data[0];
		Ship->AvoidBoidsStrength = Data[1];
		Ship->CenteringStrength = Data[2];
		Ship->ObstacleAvoidanceStrength = Data[3];
		Ship->AsteroidStrength = Data[4];
		//make sure the speed is 650
		Ship->Speed = 650.0f;
	}
}

void ASpawner::HarvesterGeneration()
{
	//Use this to save all the parent data
	TArray<AHarvester*> Parents;
	bool bIsUpdateHHighestIndexNeeded = true;
	//if we got our data successfully, the data will be saved when the ship destroyed
	//loop the number of breeding times
	for (int i = 0; i < NumBreedings; i++)
	{
		// initialize the highest fitness score and the current Index
		float TempHighestFitness = 0.0f;
		int Index = -1;
		// loop through all the data that we got from the harvester ships
		for (int j = 0; j < Harvesters.Num(); j++)
		{
			//check is fitness score of the current data from ship
			if (Harvesters[j]->FitnessScore > TempHighestFitness)
			{
				//update the highest fitness score
				TempHighestFitness = Harvesters[j]->FitnessScore;
				//update index
				Index = j;
			}
		}
		if (bIsUpdateHHighestIndexNeeded)
		{
			HighestHarvesterScore = TempHighestFitness;
			HHighestIndex = Index;
			bIsUpdateHHighestIndexNeeded = false;
		}
		
		//if we got the index of highest fitness score
		if (Index != -1)
		{
			//add the detail data of it into the parents
			Parents.Add(Harvesters[Index]);
			//remove this data from the TArray
			Harvesters.RemoveAt(Index);
		}
	}
	//loop number of breeding / 2 times
	for (int i = 0; i < NumBreedings/2; i++)
	{
		if (Parents[i * 2 + 0] && Parents[i * 2 + 1])
		{
			TArray<float> Ship1Data = Parents[i * 2 + 0]->ShipData;
			TArray<float> Ship2Data = Parents[i * 2 + 1]->ShipData;
			//Spawn two harvester ship
			AHarvester* Harvester1 = SpawnHarvesterShip();
			AHarvester* Harvester2 = SpawnHarvesterShip();
			//get mid index which will be used in crossover step
			int MidIndex = FMath::RandRange(2, Parents.Num() - 2);
			TArray<float> CrossoverData1;
			TArray<float> CrossoverData2;
			//create two new data array, then doing the crossover and save the new data into those two array
			if (Ship1Data.Num() == Parents[i]->ShipData.Num() && Ship2Data.Num() == Parents[i]->ShipData.Num())
			{
				//the crossover steps
				for (int j = 0; j < Parents[i]->ShipData.Num(); j++)
				{
					//bug there are some unexpect crash happened in here (after few generations) and i cannot figure it out why
					if (j < MidIndex)
					{
						CrossoverData1.Add(Ship1Data[j]);
						CrossoverData2.Add(Ship2Data[j]);
					}
					else
					{
						CrossoverData1.Add(Ship2Data[j]);
						CrossoverData2.Add(Ship1Data[j]);
					}
				}
				//update them with mutation applied
				if (FMath::RandRange(0.0f, 1.0f) < MutationChance)
				{
					if (FMath::RandBool())
					{
						CrossoverData1 = Mutation(CrossoverData1);
					}
					else
					{
						CrossoverData2 = Mutation(CrossoverData2);
					}
				}
				//update the data with the two new ships/offsprings that we created
				SetShipVariables(Harvester1, CrossoverData1);
				SetShipVariables(Harvester2, CrossoverData2);
			}
			
			CrossoverData1.Empty();
			CrossoverData2.Empty();
		}
	}
}

void ASpawner::PirateGeneration()
{
	//Use this to save all the parent data
	TArray<APirate*> Parents;
	bool bIsUpdatePHighestIndexNeeded = true;
	//if we got our data successfully, the data will be saved when the ship destroyed
	if (Pirates.Num() > NumPirateBreedings)
	{
		//loop the number of breeding times
		for (int i = 0; i < NumPirateBreedings; i++)
		{
			// initialize the highest fitness score and the current Index
			float TempHighestFitness = 0.0f;
			int Index = -1;
			// loop through all the data that we got from the harvester ships
			for (int j = 0; j < Pirates.Num(); j++)
			{
				//check is fitness score of the current data from ship
				if (Pirates[j]->FitnessScore > TempHighestFitness)
				{
					//update the highest fitness score
					TempHighestFitness = Pirates[j]->FitnessScore;
					//update index
					Index = j;
				}
			}
			if (bIsUpdatePHighestIndexNeeded)
			{
				HighestPirateScore = TempHighestFitness;
				PHighestIndex = Index;
				bIsUpdatePHighestIndexNeeded = false;
			}
			//if we got the index of highest fitness score
			if (Index != -1)
			{
				//add the ship into the parents
				Parents.Add(Pirates[Index]);
				//remove this data from the TArray
				Pirates.RemoveAt(Index);
			}
		}	
	}
	//loop number of breeding / 2 times
	for (int i = 0; i < NumPirateBreedings/2; i++)
	{
		//Spawn two harvester ship
		APirate* PirateShip1 = SpawnPirateShip();
		APirate* PirateShip2 = SpawnPirateShip();
		//get mid index which will be used in crossover step
		int MidIndex = FMath::RandRange(2, Parents.Num() - 2);
		TArray<float> CrossoverData1;
		TArray<float> CrossoverData2;
		//create two new data array, then doing the crossover and save the new data into those two array
		for (int j = 0; j < Parents[i]->ShipData.Num(); j++)
		{
			if (j < MidIndex)
			{
				CrossoverData1.Add(Parents[i * 2 + 0]->ShipData[j]);
				CrossoverData2.Add(Parents[i * 2 + 1]->ShipData[j]);
			}
			else
			{
				CrossoverData1.Add(Parents[i * 2 + 1]->ShipData[j]);
				CrossoverData2.Add(Parents[i * 2 + 0]->ShipData[j]);
			}
		}
		//update them with mutation applied
		if (FMath::RandRange(0.0f, 1.0f) < MutationChance)
		{
			if (FMath::RandBool())
			{
				CrossoverData1 = Mutation(CrossoverData1);
			}
			else
			{
				CrossoverData2 = Mutation(CrossoverData2);
			}
		}
		//update the data with the two new ships/offsprings that we created
		SetShipVariables(PirateShip1, CrossoverData1);
		SetShipVariables(PirateShip2, CrossoverData2);
	}
}

TArray<float> ASpawner::Mutation(TArray<float> Data)
{
	//loop through all the elements
	for (int i = 0; i < Data.Num(); i++)
	{
		//if mutation need to be happened
		if (FMath::RandRange(0.0f, 1.0f) < MutationChance)
		{
			//create value randomly
			Data[i] = FMath::RandRange(1.0f, 10000.0f);
			if (i == Data.Num() - 1)
			{
				//just different range for speed
				Data[i] = FMath::RandRange(500.0f, 1000.0f);
			}
		}
	}
	return Data;
}

