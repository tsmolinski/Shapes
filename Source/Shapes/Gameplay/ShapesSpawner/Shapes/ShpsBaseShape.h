// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShpsBaseShape.generated.h"

class UStaticMeshComponent;

UCLASS()
class SHAPES_API AShpsBaseShape : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShpsBaseShape();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
