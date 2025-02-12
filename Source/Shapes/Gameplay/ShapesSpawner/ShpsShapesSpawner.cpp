// Fill out your copyright notice in the Description page of Project Settings.


#include "ShpsShapesSpawner.h"

#include "Shapes/ShpsBaseShape.h"

// Sets default values
AShpsShapesSpawner::AShpsShapesSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShpsShapesSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShpsShapesSpawner::AddColor(AShpsBaseShape* Shape, int Index, TArray<FLinearColor> Colors)
{
		TObjectPtr<UStaticMeshComponent> ShapeMeshComponent = Cast<UStaticMeshComponent>(Shape->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (ShapeMeshComponent)
		{
			TObjectPtr<UMaterialInstanceDynamic> ShapeMaterialInstance = ShapeMeshComponent->CreateDynamicMaterialInstance(0);

			if (ShapeMaterialInstance)
			{
				int ColorsArrayIndex = Index % Colors.Num();
				ShapeMaterialInstance->SetVectorParameterValue(FName("Color"), Colors[ColorsArrayIndex]);
			}
		}
}

// Called every frame
void AShpsShapesSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

