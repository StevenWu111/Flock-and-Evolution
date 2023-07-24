// Fill out your copyright notice in the Description page of Project Settings.


#include "Harvester.h"

#include "AITypes.h"
#include "Pirate.h"
#include "Spawner.h"
#include "Engine/StaticMeshActor.h"

AHarvester::AHarvester()
{
	//Add those value into ship data (it is not necessary because we will update it later)
	ShipData.Add(VelocityMatchingStrength);
	ShipData.Add(AvoidBoidsStrength);
	ShipData.Add(CenteringStrength);
	ShipData.Add(ObstacleAvoidanceStrength);
	ShipData.Add(AsteroidStrength);
	ShipData.Add(Speed);
}

FVector AHarvester::AvoidBoids(TArray<AActor*> Flock)
{
	FVector AvoidVector = FVector::ZeroVector;
	//if we have other boids around us
	if (!Flock.IsEmpty())
	{
		//we loop through the boids
		for (auto Ship:Flock)
		{
			//if we confirm that they are boids, include both harvester and Pirate
			if (Cast<ABoid>(Ship))
			{
				FVector ShipLocation = Ship->GetActorLocation();
				FVector CurrentLocation = this->GetActorLocation();
				//calculate the current avoid vector
				FVector CurrentAvoidVector = CurrentLocation - ShipLocation;
				//Add them into one vector if there are multiple boids exist (Add them in unit vector)
				AvoidVector += CurrentAvoidVector.GetSafeNormal();
			}
		}
		//calculate the average of avoid vector
		AvoidVector /= Flock.Num();
		//use final - initial to get the direction in unit vector
		AvoidVector.GetSafeNormal() -= BoidVelocity.GetSafeNormal();
	}
	//return it with the strength applied
	return AvoidVector * AvoidBoidsStrength;
}

FVector AHarvester::VelocityMatching(TArray<AActor*> Flock)
{
	FVector MatchingVector = FVector::ZeroVector;
	int Count = 0;
	//if we got the boids in range
	if (!Flock.IsEmpty())
	{
		//loop through it
		for (auto Ship:Flock){
			//if it is harvester ship. Unlike the previous one, we only care about the harvester ship in this
			if (AHarvester* HarvesterShip = Cast<AHarvester>(Ship))
			{
				if (Ship != this)
				{
					//Add the vectors together (in unit vector)
					MatchingVector += HarvesterShip->GetActorForwardVector().GetSafeNormal();
					Count++;
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
	//will return 0 vector
	return MatchingVector;
}

FVector AHarvester::FlockCentering(TArray<AActor*> Flock)
{
	FVector Centeringvector = FVector::ZeroVector;
	int Count = 0;
	if (!Flock.IsEmpty())
	{
		for (auto Ship:Flock){
			//if it is harvester ship
			if (AHarvester* HarvesterShip = Cast<AHarvester>(Ship))
			{
				//Add the actor locations together
				Centeringvector += HarvesterShip->GetActorLocation();
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

FVector AHarvester::AvoidObstacle()
{
	if (IsObstacleAhead())
	{
		//Calculate the angle between the forward vector and the normal
		float Angle = FMath::Acos(FVector::DotProduct(this->GetActorForwardVector().GetSafeNormal(), ObstacleAvoidance.GetSafeNormal()));
		//Calculate the reflection vector based on the ship current vector and the normal vector of the obstacle
		//R = I - 2(I dotproduct N) * N which I is the current forward vector, N is the normal vector get from obstacle
		//I tried this fomula before, and not changed into this method. I just leave it here
		if (ObstacleAvoidance != FVector::ZeroVector)
		{
			//ObstacleAvoidance = GetActorForwardVector().GetSafeNormal() - 2*(FVector::DotProduct(GetActorForwardVector().GetSafeNormal(),ObstacleAvoidance.GetSafeNormal())*ObstacleAvoidance.GetSafeNormal());
			ObstacleAvoidance = this->GetActorForwardVector().RotateAngleAxisRad(Angle, ObstacleAvoidance);
			//return the opposite vector with the strength applied.
			return ObstacleAvoidance.GetSafeNormal()* -1 * ObstacleAvoidanceStrength;
		}
	}
	return FVector::ZeroVector;
}

FVector AHarvester::TowardAsteroid()
{
	//Check if we current have a spawner
	if (Spawner)
	{
		//if we are colliding with the Asteroid
		if (HitAsteroid)
		{
			//change it to false, which will targeting to the next Asteroid
			HitAsteroid = false;
			//increase the index by 1;
			CurrentTargetingAsteroid++;
			//when the index exceed the length of the array
			if (CurrentTargetingAsteroid >= LocationInOrder.Num())
			{
				// change it back to 0;
				CurrentTargetingAsteroid = 0;
			}
		}
		//change if we got the locations in order
		if (!LocationInOrder.IsEmpty())
		{
			//UE_LOG(LogTemp,Warning, TEXT("Is not empty"))
			//then we firstly targeting to the location which cloest to us, and once reach the asteroid, it will targeting to the next Asteroid 
			FVector TargetLocation = LocationInOrder[CurrentTargetingAsteroid];
			return (TargetLocation - GetActorLocation()).GetSafeNormal();
		}
		return FVector::ZeroVector;
	}
	return  FVector::ZeroVector;
}


void AHarvester::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//use this to save the asteroid based on the order of the distance between this ship and asteroids
	if (GetAsteroid && Spawner)
	{
		//Use to save the Location of the Asteroids
		TArray<FVector> GetAsteroidLocation;
		TArray<float> Distance;
		//Loop through the Asteroids from spawner and get their locatio
		for (auto Asteroid: Spawner->AsteroidClusters)
		{
			//Get the Asteroid locations
			GetAsteroidLocation.Add(Asteroid->GetActorLocation());
			//Get the distance between the current Asteroid and current ship
			Distance.Add(FVector::Distance(this->GetActorLocation(), Asteroid->GetActorLocation()));
		}
		//if we got the locations successfully
		if (!GetAsteroidLocation.IsEmpty() && !Distance.IsEmpty())
		{
			//use to store the current lowest distance
			float Temp = INFINITY;
			//save the current index of the asteroid with lowest distance
			int Index = 0;
			//because the length of both locations and distance are same, so I just call the for loop twice based on the distance TArray
			for (int i = 0; i < Distance.Num(); i ++)
			{
				//Use to search for the current lowest distance and the Index of it
				for (int j = 0; j < Distance.Num(); j ++)
				{
					//if the current distance is less than the previous loop
					if (Temp > Distance[j])
					{
						//update the both temp and Index variables
						Temp = Distance[j];
						Index = j;
					}
				}
				//save the location of Asteroid based ib their distance
				LocationInOrder.Add(GetAsteroidLocation[Index]);
				//Change the current distance value with Infinity which will be ignored in next loop.
				Distance[Index] = INFINITY;
			}
		}
		GetAsteroid = false;
	}
	//record the surviving time
	SurvivingTime += DeltaSeconds;
	//Calculate the actor location in next tick as well as the rotation
	FlightPath(DeltaSeconds);
	UpdateMeshRotation();
	//update data every tick (not necessary, I don't this because I met a crash on this part, just make sure the data can be collect 100%)
	ShipData[0] = VelocityMatchingStrength;
	ShipData[1] = AvoidBoidsStrength;
	ShipData[2] = CenteringStrength;
	ShipData[3] = ObstacleAvoidanceStrength;
	ShipData[4] = AsteroidStrength;
	ShipData[5] = Speed;
	FitnessScore = SurvivingTime + ResourcesCollected;
}

void AHarvester::FlightPath(float DeltaTime)
{
	Super::FlightPath(DeltaTime);
	//BoidVelocity = this->GetActorForwardVector()*100;
	//FVector Movement = (AvoidBoids(DetectedHarvester) + VelocityMatching(DetectedHarvester) + FlockCentering(DetectedHarvester) + AvoidObstacle() + GetActorForwardVector()) * DeltaTime * 100;
	//SetActorRotation(FMath::RInterpTo(GetActorRotation(), Movement.Rotation(), GetWorld()->DeltaTimeSeconds, 7.0f));
	//SetActorLocation(GetActorLocation() + Movement);
	//Get all detected ships
	TArray<AActor*> DetectedShip;
	//get all the detected ships
	PerceptionSensor->GetOverlappingActors(DetectedShip, ABoid::StaticClass());
	//update actor location
	this->SetActorLocation(GetActorLocation() + BoidVelocity * DeltaTime);
	//Just use this to make sure the actor has the correct rotation
	this->SetActorRotation(BoidVelocity.ToOrientationQuat());
	//add the vectors that we calculated together
	FVector Movement =	AvoidBoids(DetectedShip) +
						VelocityMatching(DetectedShip) +
						FlockCentering(DetectedShip) +
						AvoidObstacle() + TowardAsteroid();
	//change it with unit vector
	Movement.Normalize();
	//update the boid velocity
	BoidVelocity += Movement * Speed * DeltaTime;
	//make sure it will always have the fixed length(magnitude)
	BoidVelocity = BoidVelocity.GetClampedToSize(Speed, Speed);
}

void AHarvester::OnHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if it still in the invincible period, then we do nothing when the collision happened
	if(Invincibility <= 0)
	{
		if(OtherActor && OtherActor != this && OverlappedComponent->GetName().Equals(TEXT("Boid Collision Component")))
		{
			//update the value again, I tried to fix some bug therefore I update it many times
			ShipData[0] = VelocityMatchingStrength;
			ShipData[1] = AvoidBoidsStrength;
			ShipData[2] = CenteringStrength;
			ShipData[3] = ObstacleAvoidanceStrength;
			ShipData[4] = AsteroidStrength;
			ShipData[5] = Speed;
			FitnessScore = SurvivingTime + ResourcesCollected;
			//Check what object is the ship colliding with
			AAsteroidCluster* Asteroids = Cast<AAsteroidCluster>(OtherActor);
			//if it is the asteroids
			if(Asteroids)
			{
				//update the CollisionAsteroids to the current overlapping Asteroids
				CollisionAsteroids = Asteroids;
				HitAsteroid = true;
				return;
			}

			ABoid* OtherShip = Cast<ABoid>(OtherActor);
			//if it is another Harvester ship and collide with the inner collision component
			if(OtherShip && OtherComponent->GetName().Equals(TEXT("Boid Collision Component")))
			{
				//reduce the score 25%
				FitnessScore *= 0.75;
				//if it is a pirate ship
				if (APirate* Pirate = Cast<APirate>(OtherShip))
				{
					//Add the current resources to the pirate ship
					OtherShip->ResourcesCollected += this->ResourcesCollected;
					//make it freeze
					Pirate->StayTime = 5.0f;
					//reduce 50% fitness score
					FitnessScore *= 0.5;
				}
				Spawner->NumHarvesterShips--;
				//add this into spawner which will be used for the evaluation
				Spawner->Harvesters.Add(this);
				//UE_LOG(LogTemp,Warning,TEXT("hit ship"));
				Destroy();
				return;
			}
			//if it is wall, destroy itself
			AStaticMeshActor* Wall = Cast<AStaticMeshActor>(OtherActor);
			if(Wall)
			{
				Spawner->NumHarvesterShips--;
				//reduce 25% of the score
				FitnessScore *= 0.75;
				Spawner->Harvesters.Add(this);
				//UE_LOG(LogTemp,Warning,TEXT("hit wall"));
				Destroy();
				return;
			}
		}
	}
}
//called when collision end
void AHarvester::OnHitboxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	AAsteroidCluster* Asteroids = Cast<AAsteroidCluster>(OtherActor);
	//if collision end and it previously overlapped with Asteroids, change the CollisionAsteroids reference into nullptr
	if(Asteroids)
	{
		CollisionAsteroids = nullptr;
	}
}
