// Fill out your copyright notice in the Description page of Project Settings.


#include "Pirate.h"

#include "Spawner.h"
#include "Engine/StaticMeshActor.h"

APirate::APirate()
{
	//speed is fixed
	Speed = 650;
	BoidVelocity = GetActorForwardVector();
	//BoidVelocity*= FMath::RandRange(MinSpeed, MaxSpeed);
	BoidVelocity *= Speed;
}

FVector APirate::AvoidBoids(TArray<AActor*> Flock)
{
	FVector AvoidVector = FVector::ZeroVector;
	if (!Flock.IsEmpty())
	{
		for (auto Ship:Flock)
		{
			//we only care about the other pirate ship
			if (Cast<APirate>(Ship))
			{
				//calculate the direction by final - initial
				FVector ShipLocation = Ship->GetActorLocation();
				FVector CurrentLocation = this->GetActorLocation();
				FVector CurrentAvoidVector = CurrentLocation - ShipLocation;
				//add the unit vector of them together
				AvoidVector += CurrentAvoidVector.GetSafeNormal();
			}
		}
		//get average
		AvoidVector /= Flock.Num();
		AvoidVector.GetSafeNormal() -= BoidVelocity.GetSafeNormal();
	}
	//return it with the strength applied
	return AvoidVector * AvoidBoidsStrength;
}

FVector APirate::VelocityMatching(TArray<AActor*> Flock)
{
	
	FVector MatchingVector = FVector::ZeroVector;
	int Count = 0;
	//if we got the boids in range
	if (!Flock.IsEmpty())
	{
		//loop through it
		for (auto Ship:Flock){
			//we only care about other pirate ship
			if (APirate* PirateShip = Cast<APirate>(Ship))
			{
				if (Ship != this)
				{
					//Add the vectors together (in unit vector)
					MatchingVector += PirateShip->BoidVelocity.GetSafeNormal();
				}
			}
		}
	}
	if (Count > 0)
	{
		//get the average
		MatchingVector /= Count;
		//Get the direction in unit vector
		MatchingVector.GetSafeNormal() -= this->GetActorForwardVector().GetSafeNormal();
		//return it with the strength applied
		return MatchingVector * VelocityMatchingStrength;
	}
	return MatchingVector;
}

FVector APirate::FlockCentering(TArray<AActor*> Flock)
{
	FVector Centeringvector = FVector::ZeroVector;
	int Count = 0;
	if (!Flock.IsEmpty())
	{
		for (auto Ship:Flock){
			//if it is Pirate ship
			if (APirate* PirateShip = Cast<APirate>(Ship))
			{
				//Add the actor locations together
				Centeringvector += PirateShip->GetActorLocation();
				Count++;
			}
		}
	}
	if (Centeringvector != FVector::ZeroVector)
	{
		//calculate the average
		Centeringvector /= Count;
		//Get the direction vector
		Centeringvector -= this->GetActorLocation();
		Centeringvector.GetSafeNormal() -= this->BoidVelocity.GetSafeNormal();
		//return it with the strength applied
		return Centeringvector * CenteringStrength;
	}
	return FVector::ZeroVector;
}

FVector APirate::AvoidObstacle()
{
	if (IsObstacleAhead())
	{
		float Angle = FMath::Acos(FVector::DotProduct(this->GetActorForwardVector().GetSafeNormal(), ObstacleAvoidance.GetSafeNormal()));
		//Calculate the reflection vector based on the ship current forward vector and the normal vector of the obstacle
		//R = I - 2(I dotproduct N) * N which I is the current forward vector, N is the normal vector get from obstacle
		//I tried this fomula before, and not changed into this method. I just leave it here
		if (ObstacleAvoidance != FVector::ZeroVector)
		{
			ObstacleAvoidance = this->GetActorForwardVector().RotateAngleAxisRad(Angle, ObstacleAvoidance);
			//return it with the strength applied
			return ObstacleAvoidance.GetSafeNormal()* -1 * ObstacleAvoidanceStrength;
		}
	}
	return FVector::ZeroVector;
}

void APirate::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//we only allow it to move 
	if (StayTime <= 0)
	{
		FlightPath(DeltaSeconds);
		UpdateMeshRotation();
	}
	else
	{
		StayTime -= DeltaSeconds;
	}
	//record the surviving time
	SurvivingTime += DeltaSeconds;
	
}

void APirate::FlightPath(float DeltaTime)
{
	Super::FlightPath(DeltaTime);
	//BoidVelocity = this->GetActorForwardVector()*100;
	//FVector Movement = (AvoidBoids(DetectedHarvester) + VelocityMatching(DetectedHarvester) + FlockCentering(DetectedHarvester) + AvoidObstacle() + GetActorForwardVector()) * DeltaTime * 100;
	//SetActorRotation(FMath::RInterpTo(GetActorRotation(), Movement.Rotation(), GetWorld()->DeltaTimeSeconds, 7.0f));
	//SetActorLocation(GetActorLocation() + Movement);
	//Get all detected ships
	TArray<AActor*> DetectedShip;
	PerceptionSensor->GetOverlappingActors(DetectedShip, ABoid::StaticClass());
	this->SetActorLocation(GetActorLocation() + BoidVelocity * DeltaTime);
	this->SetActorRotation(BoidVelocity.ToOrientationQuat());
	FVector Movement =  AvoidBoids(DetectedShip) +
						VelocityMatching(DetectedShip) +
						FlockCentering(DetectedShip) +
						AvoidObstacle() + TowardHarv();
	Movement.Normalize();
	BoidVelocity += Movement * Speed * DeltaTime;
	BoidVelocity = BoidVelocity.GetClampedToSize(Speed, Speed);
}

FVector APirate::TowardHarv()
{
	//This the method that we always chasing the closest ship
	/*
	//Use this to store the ship that we current detected by sensor
	TArray<AActor*> DetectedShip;
	//Get only the harvester ship in range and save them in DetectedShip Tarray
	PerceptionSensor->GetOverlappingActors(DetectedShip, AHarvester::StaticClass());
	//When the current targeting HarvesterShip is nullptr and we there are some harvester ship in range, this will happened when the targeting ship is destroyed or the first time we spawned
	if (HarvesterShip == nullptr && !DetectedShip.IsEmpty())
	{
		//Initialize it with INFINITY make sure it large than any number in the first loop
		float Distance = INFINITY;
		//Use this to save the Index of the ship that closest to the pirate ship
		int Index = -1;
		//loop through the detectedship
		for (int i =0; i < DetectedShip.Num(); i++)
		{
			//Double check if it is a Harvester ship
			if (AHarvester* Ship = Cast<AHarvester>(DetectedShip[i]))
			{
				//Get the current distance between the ship and itself
				float CurrDistance = FVector::Distance(this->GetActorLocation(), Ship->GetActorLocation());
				//if the current harvester ship is closer
				if (CurrDistance < Distance)
				{
					//update the distance temp
					Distance = CurrDistance;
					//update the index
					Index = i;
				}
			}
		}
		//if we currently have the index saved
		if (Index >= 0)
		{
			//we know which is we are targeting to
			HarvesterShip = DetectedShip[Index];
		}
		else
		{
			//return the zerovector when there is no targeting ship
			return FVector::ZeroVector;
		}
		
	}
	if (HarvesterShip)
	{
		UE_LOG(LogTemp,Warning,TEXT("Targeting"))
		//Return the direction toward the harvester ship
		return (HarvesterShip->GetActorLocation() - this->GetActorLocation()).GetSafeNormal() * 100000000;
	}
	//return the zerovector when there is no targeting ship
	return FVector::ZeroVector;*/

	//this is the way that we chasing the first detected ship
	//Use this to store the ship that we current detected by sensor
	TArray<AActor*> DetectedShip;
	//Get only the harvester ship in range and save them in DetectedShip Tarray
	PerceptionSensor->GetOverlappingActors(DetectedShip, AHarvester::StaticClass());
	if (!DetectedShip.IsEmpty())
	{
		//chasing the first detected ship
		HarvesterShip = DetectedShip[0];
		//get the direction in unit vector
		return (HarvesterShip->GetActorLocation() - this->GetActorLocation()).GetSafeNormal();
	}
	return FVector::ZeroVector;
}

void APirate::OnHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor != this && Invincibility <= 0)
	{
		//Can only destroy by other Pirate
		APirate* OtherPirate = Cast<APirate>(OtherActor);
		if(OtherPirate && OtherPirate->Invincibility <= 0 && OtherComponent->GetName().Equals(TEXT("Boid Collision Component")))
		{
			//update the data
			Spawner->NumPirateShips--;
			FitnessScore = SurvivingTime + ResourcesCollected;
			FitnessScore *= 0.75;
			ShipData.Add(FitnessScore);
			ShipData.Add(VelocityMatchingStrength);
			ShipData.Add(AvoidBoidsStrength);
			ShipData.Add(CenteringStrength);
			ShipData.Add(ObstacleAvoidanceStrength);
			ShipData.Add(AsteroidStrength);
			ShipData.Add(Speed);
			Spawner->Pirates.Add(this);
			Destroy();
			return;
		}
		//As well as the Wall
		AStaticMeshActor* Wall = Cast<AStaticMeshActor>(OtherActor);
		if(Wall)
		{
			//update the data
			Spawner->NumPirateShips--;
			FitnessScore = SurvivingTime + ResourcesCollected;
			FitnessScore *= 0.75;
			ShipData.Add(FitnessScore);
			ShipData.Add(VelocityMatchingStrength);
			ShipData.Add(AvoidBoidsStrength);
			ShipData.Add(CenteringStrength);
			ShipData.Add(ObstacleAvoidanceStrength);
			ShipData.Add(AsteroidStrength);
			ShipData.Add(Speed);
			Spawner->Pirates.Add(this);
			Destroy();
			return;
		}
	}
}
