// Fill out your copyright notice in the Description page of Project Settings.


#include "ShpsTooltipWidget.h"

void UShpsTooltipWidget::SetSelectableInterfaceActor(TScriptInterface<UShpsSelectableInterface> Other)
{
	SelectableInterfaceActor = Other;
}
