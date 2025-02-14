// Fill out your copyright notice in the Description page of Project Settings.


#include "ShpsBaseShape.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AShpsBaseShape::AShpsBaseShape()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(StaticMeshComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetDrawAtDesiredSize(true);
}

FText AShpsBaseShape::GetPrimitiveType()
{
	return PrimitiveType;
}

FText AShpsBaseShape::GetPrimitiveColor()
{
	return PrimitiveColor;
}

FText AShpsBaseShape::GetPrimitiveSize()
{
	return PrimitiveSize;
}

void AShpsBaseShape::SetPrimitiveType(const TSubclassOf<AShpsBaseShape>& Primitive, TMap<TSubclassOf<AShpsBaseShape>, FText> Primitives)
{
	PrimitiveType = *(Primitives.Find(Primitive));
}

void AShpsBaseShape::SetPrimitiveColor(const FLinearColor& Color, TMap<FLinearColor, FText> Colors)
{
	PrimitiveColor = *(Colors.Find(Color));
}

void AShpsBaseShape::SetPrimitiveSize(AShpsBaseShape* Shape)
{
	FVector MinVec, MaxVec;
	Shape->StaticMeshComponent->GetLocalBounds(MinVec, MaxVec);
	FVector Size = MinVec.GetAbs() + MaxVec;
	Size = Size * StaticMeshComponent->GetComponentScale();

	PrimitiveSize = Size.ToText();
}

void AShpsBaseShape::SelectPrimitive_Implementation()
{
	WidgetComponent->SetVisibility(true);
}

void AShpsBaseShape::UnselectPrimitive_Implementation()
{
	WidgetComponent->SetVisibility(false);
}

FText AShpsBaseShape::GetType_Implementation()
{
	return GetPrimitiveType();
}

FText AShpsBaseShape::GetColor_Implementation()
{
	return GetPrimitiveColor();
}

FText AShpsBaseShape::GetSize_Implementation()
{
	return GetPrimitiveSize();
}

// Called when the game starts or when spawned
void AShpsBaseShape::BeginPlay()
{
	Super::BeginPlay();

	WidgetComponent->SetVisibility(false);
}

// Called every frame
void AShpsBaseShape::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

