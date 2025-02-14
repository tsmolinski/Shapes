// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShpsTooltipWidget.generated.h"

/**
 * 
 */

class UShpsSelectableInterface;

UCLASS()
class SHAPES_API UShpsTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TScriptInterface<UShpsSelectableInterface> SelectableInterfaceActor;
	
};
