// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ShpsSelectableInterface.generated.h"

//class AShpsBaseShape;
class AACtor;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UShpsSelectableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SHAPES_API IShpsSelectableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SelectPrimitive();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UnselectPrimitive();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetType();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetColor();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetSize();
};
