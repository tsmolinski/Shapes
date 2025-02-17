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
	
	AShpsBaseShape* SpawnShapeInRandomLocAndSize(const TSubclassOf<AShpsBaseShape>& Primitive);
	
	AShpsBaseShape* ChangePrimitiveType(const TSubclassOf<AShpsBaseShape>& PrimitiveType, AShpsBaseShape* Shape);
	
	void AddColorsToShapes(TArray<AShpsBaseShape*> Shapes, const TMap<FLinearColor, FText>& Colors);

	void AddColorToShape(AShpsBaseShape* BaseShape, const FLinearColor& Color);

	void OnRandomNumberGenerated(int Number);

	void InitSpawner();

	bool SameNumberOfEachPrimitive(TMap<FString, int>& PrimitivesNum);

	bool SameNumberOfEachColor(TMap<FString, int>& ColorsNum);

	TArray<FString> PrimitivesTypeAboveToleranceNumber(TMap<FString, int>& PrimitivesNum, FText& DestroyedPrimitiveType);

	TArray<FString> ColorsAboveToleranceNumber(TMap<FString, int>& ColorsNum, FText& DestroyedPrimitiveColor);

	void UpdatePrimitivesNumMap(TMap<FString, int>& PrimitivesNum);

	void UpdateColorsNumMap(TMap<FString, int>& ColorsNum);

	const FString* GetPrimitiveTypeLargestQuantity() const;

	const FString* GetPrimitiveTypeLeastQuantity() const;

	const FString* GetColorLargestQuantity() const;

	const FString* GetColorLeastQuantity() const;
	
	UFUNCTION()
	void OnShapeShooted(AActor* BaseShapeActor);

	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(EditDefaultsOnly)
	int ToleranceNumber = 1;

	UPROPERTY()
	int RandomNumber = 1;
	
	UPROPERTY(EditAnywhere, Category = "Arrays")
	TMap<FLinearColor, FText> ColorsMap;

	UPROPERTY()
	TMap<FLinearColor, FString> ColorsMapString;

	UPROPERTY(EditAnywhere, Category = "Arrays")
	TMap<TSubclassOf<AShpsBaseShape>, FText> PrimitivesMap;

	UPROPERTY()
	TMap<TSubclassOf<AShpsBaseShape>, FString> PrimitivesMapString;
	
	UPROPERTY(EditDefaultsOnly,Category = "Arrays")
	TArray<TObjectPtr<AShpsBaseShape>> ShapesArray;

	UPROPERTY()
	TMap<FString, int> PrimitivesNumMap;

	UPROPERTY()
	TMap<FString, int> ColorsNumMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};