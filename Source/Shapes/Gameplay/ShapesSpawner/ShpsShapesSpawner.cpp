// Fill out your copyright notice in the Description page of Project Settings.


#include "ShpsShapesSpawner.h"
#include "Shapes/ShpsBaseShape.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Shapes/Core/GameMode/ShpsGameModeBase.h"

#include "Containers/Map.h"

// Sets default values
AShpsShapesSpawner::AShpsShapesSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(StaticMeshComponent);
}

// Called when the game starts or when spawned
void AShpsShapesSpawner::BeginPlay()
{
	Super::BeginPlay();

	AShpsGameModeBase* GameModeBase =  Cast<AShpsGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameModeBase)
	{
		GameModeBase->OnRandomNumberGeneratedDelegate.AddUObject(this, &AShpsShapesSpawner::OnRandomNumberGenerated);
	}
}

AShpsBaseShape* AShpsShapesSpawner::SpawnShapeInRandomLocAndSize(const TSubclassOf<AShpsBaseShape>& Primitive)
{
	TObjectPtr<UWorld> World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
				
		FVector BoxLocation = BoxComponent->GetComponentLocation();
		FVector BoxExtent = BoxComponent->GetUnscaledBoxExtent();
		FVector RandomLocationInBox = UKismetMathLibrary::RandomPointInBoundingBox(BoxLocation, BoxExtent);
				
		float RandomSizeFloat = UKismetMathLibrary::RandomFloatInRange(0.5, 2.5);
		FVector RandomSize = FVector(RandomSizeFloat);

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(RandomLocationInBox);
		SpawnTransform.SetScale3D(RandomSize);

		AShpsBaseShape* SpawnedShape = World->SpawnActor<AShpsBaseShape>(Primitive, SpawnTransform, SpawnParams);
		if (SpawnedShape)
		{
			SpawnedShape->SetPrimitiveType(Primitive, PrimitivesMap);
			return SpawnedShape;
		}
	}
	return nullptr;
}

void AShpsShapesSpawner::AddColorsToShapes(TArray<AShpsBaseShape*> Shapes, const TMap<FLinearColor, FText>& Colors)
{
	int Index = 0;
	TArray<FLinearColor> ColorsArray;
	Colors.GenerateKeyArray(ColorsArray);
	
	for (auto& Shape : Shapes)
	{
		TObjectPtr<UStaticMeshComponent> ShapeMeshComponent = Cast<UStaticMeshComponent>(Shape->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (ShapeMeshComponent)
		{
			Material = ShapeMeshComponent->GetMaterial(0);
			if (Material)
			{
				TObjectPtr<UMaterialInstanceDynamic> MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(Material, Shape);
				
				if (MaterialInstanceDynamic)
				{
					ShapeMeshComponent->SetMaterial(0, MaterialInstanceDynamic);
					
					int ColorsArrayIndex = Index % ColorsArray.Num();
					MaterialInstanceDynamic->SetVectorParameterValue(FName("Color"), ColorsArray[ColorsArrayIndex]);
					Shape->SetPrimitiveColor(ColorsArray[ColorsArrayIndex], Colors);
					Shape->SetPrimitiveSize(Shape);
					++Index;
				}
				
			}
		}
	}
}

void AShpsShapesSpawner::OnRandomNumberGenerated(int Number)
{
	RandomNumber = Number;

	UE_LOG(LogTemp, Warning, TEXT("The AShpsShapesSpawner::RandomNumber value is: %d"), RandomNumber);

	InitSpawner();
}

void AShpsShapesSpawner::InitSpawner()
{
	for (auto& Primitive : PrimitivesMap)
	{
		for (int i = 0; i < RandomNumber; i++)
		{
			TObjectPtr<AShpsBaseShape> SpawnedShape = SpawnShapeInRandomLocAndSize(Primitive.Key);
			ShapesArray.Add(SpawnedShape);
		}
	}

	AddColorsToShapes(ShapesArray, ColorsMap);
}

// Called every frame
void AShpsShapesSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

