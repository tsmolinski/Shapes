// Fill out your copyright notice in the Description page of Project Settings.


#include "ShpsGameModeBase.h"

void AShpsGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GenerateRandomNumber(MinNumber, MaxNumber);
}

void AShpsGameModeBase::GenerateRandomNumber(int Min, int Max)
{
	int RandomNumber = FMath::RandRange(Min, Max);
	
	UE_LOG(LogTemp, Warning, TEXT("The AShpsGameModeBase::RandomNumber value is: %d"), RandomNumber);

	OnRandomNumberGeneratedDelegate.Broadcast(RandomNumber);
}
