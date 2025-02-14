// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShpsBaseShape.generated.h"

class UStaticMeshComponent;
class FText;

UCLASS()
class SHAPES_API AShpsBaseShape : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShpsBaseShape();

	UFUNCTION(BlueprintCallable)
	FText GetPrimitiveType();

	UFUNCTION(BlueprintCallable)
	FText GetPrimitiveColor();

	FText* GetPrimitiveSize();

	//void SetPrimitiveType(AShpsBaseShape* Shape, TMap<TSubclassOf<AShpsBaseShape>, FText> Primitives);
	void SetPrimitiveType(const TSubclassOf<AShpsBaseShape>& Primitive, TMap<TSubclassOf<AShpsBaseShape>, FText> Primitives);

	void SetPrimitiveColor(const FLinearColor& Color, TMap<FLinearColor, FText> Colors);

	void SetPrimitiveSize(AShpsBaseShape* Shape);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText PrimitiveType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText PrimitiveColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText PrimitiveSize;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
