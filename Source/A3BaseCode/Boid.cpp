
// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"

#include "Harvester.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABoid::ABoid()
{
	PrimaryActorTick.bCanEverTick = true;
	//Setup mesh component
	BoidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Boid Mesh Component"));
	BoidMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoidMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	//Setup collision sphere
	BoidCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Boid Collision Component"));
	BoidCollision->SetCollisionObjectType(ECC_Pawn);
	BoidCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoidCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoidCollision->SetSphereRadius(100.0f);
	//Attach to the RootComponent
	RootComponent = BoidMesh;
	BoidCollision->SetupAttachment(RootComponent);
	//Setup perception sensor
	PerceptionSensor = CreateDefaultSubobject<USphereComponent>(TEXT("Perception Sensor Component"));
	PerceptionSensor->SetupAttachment(RootComponent);
	PerceptionSensor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PerceptionSensor->SetCollisionResponseToAllChannels(ECR_Ignore);
	PerceptionSensor->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PerceptionSensor->SetSphereRadius(1000.0f);
	//Initialize the BoidVelocity with zeroVector
	BoidVelocity = FVector::ZeroVector;
	
	AvoidanceSensors.Empty();
	
	FVector SensorDirection;
	//place the sensor in front of each actor
	for (int i = 0; i < NumSensors; ++i)
	{
		float Yaw = 2 * UKismetMathLibrary::GetPI() * GoldenRatio * i;
		float Pitch = FMath::Acos(1 - (2 * float(i) / NumSensors));
		
		SensorDirection.X = FMath::Cos(Yaw)*FMath::Sin(Pitch);
		SensorDirection.Y = FMath::Sin(Yaw)*FMath::Sin(Pitch);
		SensorDirection.Z = FMath::Cos(Pitch);

		AvoidanceSensors.Emplace(SensorDirection);
	}
	//initialize the ship with random value of each strength
	AsteroidStrength = FMath::RandRange(MinStrength, MaxStrength);
	CenteringStrength = FMath::RandRange(MinStrength, MaxStrength);
	AvoidBoidsStrength = FMath::RandRange(MinStrength, MaxStrength);
	ObstacleAvoidanceStrength = FMath::RandRange(MinStrength, MaxStrength);
	VelocityMatchingStrength = FMath::RandRange(MinStrength, MaxStrength);
	Speed = FMath::RandRange(500, 1000);
}

void ABoid::BeginPlay()
{
	Super::BeginPlay();
	//Randomises the SpawnRotation
	FRotator SpawnRotation(FMath::RandRange(-180,180),FMath::RandRange(-180,180),FMath::RandRange(-180,180));
	//Make the boid facing toward the random rotation that we generated before
	SetActorRotation(SpawnRotation);
	//Initialize the variables with current facing direction
	CurrentRotation = GetActorRotation();
	BoidVelocity = GetActorForwardVector();
	//BoidVelocity*= FMath::RandRange(MinSpeed, MaxSpeed);
	BoidVelocity *= Speed;
	//Bind Overlap events with functions
	BoidCollision->OnComponentBeginOverlap.AddDynamic(this, &ABoid::OnHitboxOverlapBegin);
	BoidCollision->OnComponentEndOverlap.AddDynamic(this, &ABoid::OnHitboxOverlapEnd);
	
}
// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	//if there is a Collision Asteroid
	if(CollisionAsteroids)
	{
		//Update the data in ResourcesCollected based on the mount of resources that Asteroids has
		ResourcesCollected += CollisionAsteroids->RemoveGold();
	}
	//Check and update the Invincibility by deltatime
	if(Invincibility > 0)
	{
		Invincibility-= DeltaTime;
	}
}

void ABoid::UpdateMeshRotation()
{
	CurrentRotation = FMath::RInterpTo(CurrentRotation, GetActorRotation(), GetWorld()->DeltaTimeSeconds, 7.0f);
	SetActorRotation(CurrentRotation);
}

FVector ABoid::AvoidBoids(TArray<AActor*> Flock)
{
	return FVector::ZeroVector;
}

FVector ABoid::VelocityMatching(TArray<AActor*> Flock)
{
	return FVector::ZeroVector;
}

FVector ABoid::FlockCentering(TArray<AActor*> Flock)
{
	return FVector::ZeroVector;
}

FVector ABoid::AvoidObstacle()
{
	return FVector::ZeroVector;
}

void ABoid::FlightPath(float DeltaTime)
{
	
}
//Check is there any obstacle in front of boid by using line trace
bool ABoid::IsObstacleAhead()
{
	// Just tried to solve the problem with multiple sensor applied
	/*
	int Count = 0;
	if (AvoidanceSensors.Num() > 0)
	{
		for (auto Sensors: AvoidanceSensors)
		{
			FCollisionQueryParams TraceParameters;
			FHitResult Hit;
			FQuat Rotation = FQuat::FindBetweenVectors(Sensors,this->GetActorForwardVector());
			FVector Direction = Rotation.RotateVector(Sensors);
			GetWorld()->LineTraceSingleByChannel(Hit,this->GetActorLocation(),this->GetActorLocation() + Direction * SensorRadius,ECC_GameTraceChannel1,TraceParameters);
			if (Hit.bBlockingHit)
			{
				DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + Direction * SensorRadius, FColor::Red, false, -1.0f, 0, 1.5f);
				TArray<AActor*> OverlapActors;
				BoidCollision->GetOverlappingActors(OverlapActors);
				for (AActor* OverlapActor : OverlapActors)
				{
					//if the line traced actor is currently overlap with the boid return false
					if (Hit.GetActor() == OverlapActor)
					{
						return false;
					}
				}
				ObstacleAvoidance += Hit.Normal;
				Count++;
			}
		}
		if (Count > 0)
		{
			return true;
		}
	}
	return false;*/
	if (AvoidanceSensors.Num() > 0)
	{
		FCollisionQueryParams TraceParameters;
		FHitResult Hit;
		//create line trace and save the result in hit
		GetWorld()->LineTraceSingleByChannel(Hit,this->GetActorLocation(),this->GetActorLocation() + GetActorForwardVector() * SensorRadius,ECC_GameTraceChannel1,TraceParameters);
		//if the line hit something
		if (Hit.bBlockingHit)
		{
			DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + GetActorForwardVector() * SensorRadius, FColor::Red, false, -1.0f, 0, 1.5f);
			TArray<AActor*> OverlapActors;
			BoidCollision->GetOverlappingActors(OverlapActors);
			for (AActor* OverlapActor : OverlapActors)
			{
				//if the line traced actor is currently overlap with the boid return false
				if (Hit.GetActor() == OverlapActor)
				{
					return false;
				}
				ObstacleAvoidance = Hit.Normal;
			}
		}
		//else return the boolean hit result
		return Hit.bBlockingHit;
	}
	return false;
}

void ABoid::OnHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ABoid::OnHitboxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	
}

