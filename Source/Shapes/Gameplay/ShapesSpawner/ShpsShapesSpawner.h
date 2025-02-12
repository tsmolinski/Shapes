// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShpsShapesSpawner.generated.h"

class AShpsBaseShape;

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
	void AddColor(AShpsBaseShape* Shape, int Index, TArray<FLinearColor> Colors);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrays")
	TArray<FLinearColor> ColorsArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrays")
	TArray<TSubclassOf<AShpsBaseShape>> PrimitivesArray;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrays")
	TArray<TObjectPtr<AShpsBaseShape>> ShapesArray;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
