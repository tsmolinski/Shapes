// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShpsGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SHAPES_API AShpsGameModeBase : public AGameModeBase
{
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnRandomNumberGeneratedSignature, int);
	
	GENERATED_BODY()

public:
	FOnRandomNumberGeneratedSignature OnRandomNumberGeneratedDelegate;

protected:
	virtual void BeginPlay() override;

	void GenerateRandomNumber(int Min, int Max);

	UPROPERTY(EditDefaultsOnly)
	int MinNumber = 4;
	
	UPROPERTY(EditDefaultsOnly)
	int MaxNumber = 7;
};
