// Fill out your copyright notice in the Description page of Project Settings.


#include "AsteroidCluster.h"

#include "NiagaraFunctionLibrary.h"

// Sets default values
AAsteroidCluster::AAsteroidCluster()
{
	PrimaryActorTick.bCanEverTick = true;
	//Set the RootComponent and hitbox
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	AsteroidHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Hitbox"));
	AsteroidHitbox->SetupAttachment(RootComponent);

}

float AAsteroidCluster::RemoveGold()
{
	//if the current AsteroidCluster has gold
	if(ResourceAmount > 0)
	{
		//Calculate the new remaining gold by delete the gold that ship can take each time
		ResourceAmount-=AmountToTake;
		//return the amount of gold that ship can take for each time
		return AmountToTake;
	}
	return 0;
}

// Called when the game starts or when spawned
void AAsteroidCluster::BeginPlay()
{
	Super::BeginPlay();
	if(AsteroidParticle)
	{
		//Spawn the particle system
		UNiagaraFunctionLibrary::SpawnSystemAttached(AsteroidParticle, AsteroidHitbox, TEXT("Hitbox"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
	}
}

// Called every frame
void AAsteroidCluster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//If current actor has no gold left
	if(ResourceAmount <= 0)
	{
		//change the actor location randomly and set the gold amount back to 1000
		SetActorLocation(FVector(FMath::RandRange(0.0f, 5000.0f),FMath::RandRange(-2500.0f, 2500.0f),FMath::RandRange(-2500.0f, 2500.0f)));
		ResourceAmount = 1000;
	}

}

