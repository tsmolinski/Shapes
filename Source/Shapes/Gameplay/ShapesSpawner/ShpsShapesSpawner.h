// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShpsShapesSpawner.generated.h"

class AShpsBaseShape;
class UBoxComponent;
class UMaterialInterface;

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

	UFUNCTION(BlueprintCallable)
	void SpawnShapesInRandomLocAndSize(TArray<TSubclassOf<AShpsBaseShape>> Primitives, int RandomAmountGenerated);
	
	UFUNCTION(BlueprintCallable)
	void AddColorsToShapes(TArray<AShpsBaseShape*> Shapes, TArray<FLinearColor> Colors);

	TObjectPtr<UMaterialInterface> Material;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrays")
	TArray<FLinearColor> ColorsArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrays")
	TArray<TSubclassOf<AShpsBaseShape>> PrimitivesArray;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrays")
	TArray<TObjectPtr<AShpsBaseShape>> ShapesArray;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
