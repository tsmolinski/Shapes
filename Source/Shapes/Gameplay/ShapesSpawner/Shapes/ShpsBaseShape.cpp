// Fill out your copyright notice in the Description page of Project Settings.


#include "ShpsBaseShape.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AShpsBaseShape::AShpsBaseShape()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;
}

// Called when the game starts or when spawned
void AShpsBaseShape::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AShpsBaseShape::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

