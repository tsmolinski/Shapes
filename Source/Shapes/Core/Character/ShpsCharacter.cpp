// Fill out your copyright notice in the Description page of Project Settings.


#include "ShpsCharacter.h"

// Sets default values
AShpsCharacter::AShpsCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShpsCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShpsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShpsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

