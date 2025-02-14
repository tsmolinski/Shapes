// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShpsShapesSpawner.generated.h"

class AShpsBaseShape;
class UBoxComponent;
class UMaterialInterface;
//class TMap;

UCLASS()
class SHAPES_API AShpsShapesSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShpsShapesSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	AShpsBaseShape* SpawnShapeInRandomLocAndSize(const TSubclassOf<AShpsBaseShape>& Primitive);
	
	void AddColorsToShapes(TArray<AShpsBaseShape*> Shapes, const TMap<FLinearColor, FText>& Colors);

	void OnRandomNumberGenerated(int Number);

	void InitSpawner();

	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY()
	int RandomNumber = 1;
	
	UPROPERTY(EditAnywhere, Category = "Arrays")
	TMap<FLinearColor, FText> ColorsMap;

	UPROPERTY(EditAnywhere, Category = "Arrays")
	TMap<TSubclassOf<AShpsBaseShape>, FText> PrimitivesMap;
	
	UPROPERTY(EditDefaultsOnly,Category = "Arrays")
	TArray<TObjectPtr<AShpsBaseShape>> ShapesArray;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
