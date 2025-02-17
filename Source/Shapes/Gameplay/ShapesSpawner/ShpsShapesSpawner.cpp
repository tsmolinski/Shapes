// Fill out your copyright notice in the Description page of Project Settings.


#include "ShpsShapesSpawner.h"
#include "Shapes/ShpsBaseShape.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Shapes/Core/GameMode/ShpsGameModeBase.h"
#include "Containers/Map.h"
#include "Shapes/Core/Character/ShpsCharacter.h"

// Sets default values
AShpsShapesSpawner::AShpsShapesSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(StaticMeshComponent);
}

// Called when the game starts or when spawned
void AShpsShapesSpawner::BeginPlay()
{
	Super::BeginPlay();

	TObjectPtr<AShpsGameModeBase> GameModeBase =  Cast<AShpsGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameModeBase)
	{
		GameModeBase->OnRandomNumberGeneratedDelegate.AddUObject(this, &AShpsShapesSpawner::OnRandomNumberGenerated);
	}

	TObjectPtr<AShpsCharacter> PlayerCharacter = Cast<AShpsCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacter)
	{
		PlayerCharacter->OnShapeShootedDelegate.AddDynamic(this, &AShpsShapesSpawner::OnShapeShooted);
	}

	for (const auto& Color : ColorsMap)
	{
		ColorsMapString.Add(Color.Key, Color.Value.ToString());
	}

	for (const auto& Primitive : PrimitivesMap)
	{
		PrimitivesMapString.Add(Primitive.Key, Primitive.Value.ToString());
	}
}

AShpsBaseShape* AShpsShapesSpawner::SpawnShapeInRandomLocAndSize(const TSubclassOf<AShpsBaseShape>& Primitive)
{
	TObjectPtr<UWorld> World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
				
		FVector BoxLocation = BoxComponent->GetComponentLocation();
		FVector BoxExtent = BoxComponent->GetUnscaledBoxExtent();
		FVector RandomLocationInBox = UKismetMathLibrary::RandomPointInBoundingBox(BoxLocation, BoxExtent);
				
		float RandomSizeFloat = UKismetMathLibrary::RandomFloatInRange(0.5, 2.5);
		FVector RandomSize = FVector(RandomSizeFloat);

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(RandomLocationInBox);
		SpawnTransform.SetScale3D(RandomSize);

		AShpsBaseShape* SpawnedShape = World->SpawnActor<AShpsBaseShape>(Primitive, SpawnTransform, SpawnParams);
		if (SpawnedShape)
		{
			return SpawnedShape;
		}
	}
	return nullptr;
}

AShpsBaseShape* AShpsShapesSpawner::ChangePrimitiveType(const TSubclassOf<AShpsBaseShape>& PrimitiveType, AShpsBaseShape* Shape)
{
	TObjectPtr<UWorld> World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Shape->GetActorLocation());
		SpawnTransform.SetScale3D(Shape->GetActorScale());

		AShpsBaseShape* SpawnedShape = World->SpawnActor<AShpsBaseShape>(PrimitiveType, SpawnTransform, SpawnParams);
		if (SpawnedShape)
		{
			return SpawnedShape;
		}
	}
	return nullptr;
}

void AShpsShapesSpawner::AddColorsToShapes(TArray<AShpsBaseShape*> Shapes, const TMap<FLinearColor, FText>& Colors)
{
	int Index = 0;
	TArray<FLinearColor> ColorsArray;
	Colors.GenerateKeyArray(ColorsArray);
	
	for (auto& Shape : Shapes)
	{
		TObjectPtr<UStaticMeshComponent> ShapeMeshComponent = Cast<UStaticMeshComponent>(Shape->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (ShapeMeshComponent)
		{
			Material = ShapeMeshComponent->GetMaterial(0);
			if (Material)
			{
				Shape->ShapeMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(Material, Shape);
				
				if (Shape->ShapeMaterialInstanceDynamic)
				{
					ShapeMeshComponent->SetMaterial(0, Shape->ShapeMaterialInstanceDynamic);
					
					int ColorsArrayIndex = Index % ColorsArray.Num();
					Shape->ShapeMaterialInstanceDynamic->SetVectorParameterValue(FName("Color"), ColorsArray[ColorsArrayIndex]);
					Shape->SetPrimitiveColorInfo(ColorsArray[ColorsArrayIndex], Colors);
					++Index;
				}
				
			}
		}
	}
}

void AShpsShapesSpawner::AddColorToShape(AShpsBaseShape* BaseShape, const FLinearColor& Color)
{
	if (!BaseShape->ShapeMaterialInstanceDynamic)
	{
		TObjectPtr<UStaticMeshComponent> ShapeMeshComponent = Cast<UStaticMeshComponent>(BaseShape->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (ShapeMeshComponent)
		{
			Material = ShapeMeshComponent->GetMaterial(0);
			if (Material)
			{
				BaseShape->ShapeMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(Material, BaseShape);
				
				if (BaseShape->ShapeMaterialInstanceDynamic)
				{
					ShapeMeshComponent->SetMaterial(0, BaseShape->ShapeMaterialInstanceDynamic);
					
					BaseShape->ShapeMaterialInstanceDynamic->SetVectorParameterValue(FName("Color"), Color);
				}
				
			}
		}
	}
	else
	{
		BaseShape->ShapeMaterialInstanceDynamic->SetVectorParameterValue(FName("Color"), Color);
	}
}

void AShpsShapesSpawner::OnRandomNumberGenerated(int Number)
{
	RandomNumber = Number;

	UE_LOG(LogTemp, Warning, TEXT("The AShpsShapesSpawner::RandomNumber value is: %d"), RandomNumber);

	InitSpawner();
	UpdatePrimitivesNumMap(PrimitivesNumMap);
	UpdateColorsNumMap(ColorsNumMap);
}

void AShpsShapesSpawner::InitSpawner()
{
	for (auto& Primitive : PrimitivesMap)
	{
		for (int i = 0; i < RandomNumber; i++)
		{
			TObjectPtr<AShpsBaseShape> SpawnedShape = SpawnShapeInRandomLocAndSize(Primitive.Key);
			ShapesArray.Add(SpawnedShape);
			SpawnedShape->SetPrimitiveTypeInfo(Primitive.Key, PrimitivesMap);
			SpawnedShape->SetPrimitiveSizeInfo();
		}
	}

	AddColorsToShapes(ShapesArray, ColorsMap);
}

bool AShpsShapesSpawner::SameNumberOfEachPrimitive(TMap<FString, int>& PrimitivesNum)
{
	for (const auto& PrimitiveNum : PrimitivesNum)
	{
		for (const auto& PrimitiveNum2 : PrimitivesNum)
			if (PrimitiveNum.Value != PrimitiveNum2.Value)
			{
				return false;
			}
	}
	
	return true;
}

bool AShpsShapesSpawner::SameNumberOfEachColor(TMap<FString, int>& ColorsNum)
{
	for (const auto& ColorNum : ColorsNum)
	{
		for (const auto& ColorNum2 : ColorsNum)
			if (ColorNum.Value != ColorNum2.Value)
			{
				return false;
			}
	}
	
	return true;
}

TArray<FString> AShpsShapesSpawner::PrimitivesTypeAboveToleranceNumber(TMap<FString, int>& PrimitivesNum, FText& DestroyedPrimitiveType)
{
	TArray<FString> PrimitiveTypeOverrepresentedArray;
	UpdatePrimitivesNumMap(PrimitivesNumMap);
	
	for (const auto& PrimitiveNum : PrimitivesNum)
	{
		if (abs((PrimitiveNum.Value - (*PrimitivesNum.Find(DestroyedPrimitiveType.ToString())))) > ToleranceNumber)
		{
			PrimitiveTypeOverrepresentedArray.Add(PrimitiveNum.Key);
			UE_LOG(LogTemp, Warning, TEXT("Za duzo Primitives: %s"), *PrimitiveNum.Key);
		}
	}

	return PrimitiveTypeOverrepresentedArray;
}

TArray<FString> AShpsShapesSpawner::ColorsAboveToleranceNumber(TMap<FString, int>& ColorsNum, FText& DestroyedPrimitiveColor)
{
	TArray<FString> PrimitiveColorOverrepresentedArray;
	UpdateColorsNumMap(ColorsNumMap);
	
	for (const auto& ColorNum : ColorsNum)
	{
		if (abs((ColorNum.Value - (*ColorsNum.Find(DestroyedPrimitiveColor.ToString())))) > ToleranceNumber)
		{
			PrimitiveColorOverrepresentedArray.Add(ColorNum.Key);
			UE_LOG(LogTemp, Warning, TEXT("Za duzo Koloru: %s"), *ColorNum.Key);
		}
	}

	return PrimitiveColorOverrepresentedArray;
}

UE_DISABLE_OPTIMIZATION

void AShpsShapesSpawner::UpdatePrimitivesNumMap(TMap<FString, int>& PrimitivesNum)
{
	for (auto& Primitive : PrimitivesMap)
	{
		int Index = 0;
		for (auto& Shape : ShapesArray)
		{
			if (Primitive.Value.EqualTo(Shape->GetPrimitiveType()))
			{
				++Index;
				PrimitivesNum.Add(Shape->GetPrimitiveType().ToString(), Index);
			}
		}
		if (Index == 0)
		{
			PrimitivesNum.Add(Primitive.Value.ToString(), Index);
		}
	}
}
UE_ENABLE_OPTIMIZATION

void AShpsShapesSpawner::UpdateColorsNumMap(TMap<FString, int>& ColorsNum)
{
	for (auto& Color : ColorsMap)
	{
		int Index = 0;
		for (auto& Shape : ShapesArray)
		{
			if (Color.Value.EqualTo(Shape->GetPrimitiveColor()))
			{
				++Index;
				ColorsNum.Add(Shape->GetPrimitiveColor().ToString(), Index);
			}
		}
		if (Index == 0)
		{
			ColorsNum.Add(Color.Value.ToString(), Index);
		}
	}

	
}

void AShpsShapesSpawner::OnShapeShooted(AActor* BaseShapeActor)
{
	UE_LOG(LogTemp, Warning, TEXT("The AShpsShapesSpawner::OnShapeShooted: %s"), *BaseShapeActor->GetName());

	TObjectPtr<AShpsBaseShape> DestroyedBaseShape = Cast<AShpsBaseShape>(BaseShapeActor);
	FText DestroyedPrimitiveType = DestroyedBaseShape->GetPrimitiveType();
	FText DestroyedPrimitiveColor = DestroyedBaseShape->GetPrimitiveColor();
	
	ShapesArray.Remove(DestroyedBaseShape);
	DestroyedBaseShape->Destroy();

	UpdateColorsNumMap(ColorsNumMap);
	UpdatePrimitivesNumMap(PrimitivesNumMap);

	TArray<FString> PrimitiveTypeOverrepresented = PrimitivesTypeAboveToleranceNumber(PrimitivesNumMap, DestroyedPrimitiveType);
	TArray<FString> PrimitiveColorOverrepresented = ColorsAboveToleranceNumber(ColorsNumMap, DestroyedPrimitiveColor);
	
	TObjectPtr<AShpsBaseShape> ShapeToDelete;
	TObjectPtr<AShpsBaseShape> ShapeToAdd;

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//Find the primitive that has the largest quantity.
	int ResultPrimitiveMax = *PrimitivesNumMap.Find(DestroyedPrimitiveType.ToString());
	for (const auto& Primitive : PrimitivesNumMap)
	{
		if (ResultPrimitiveMax < Primitive.Value)
		{
			ResultPrimitiveMax = Primitive.Value;
		}
	}
	const FString PrimitiveMaxTypeString = *PrimitivesNumMap.FindKey(ResultPrimitiveMax);
	UE_LOG(LogTemp, Warning, TEXT("PrimitiveMaxTypeString: %s"), *PrimitiveMaxTypeString);

	//Find the primitive that has the least quantity.
	int ResultPrimitiveMin = *PrimitivesNumMap.Find(DestroyedPrimitiveType.ToString());
	for (const auto& Primitive : PrimitivesNumMap)
	{
		if (ResultPrimitiveMin > Primitive.Value)
		{
			ResultPrimitiveMin = Primitive.Value;
		}
	}
	const FString PrimitiveMinTypeString = *PrimitivesNumMap.FindKey(ResultPrimitiveMin);
	UE_LOG(LogTemp, Warning, TEXT("PrimitiveMinTypeString: %s"), *PrimitiveMinTypeString);
	
	//Find the color that has the largest quantity
	int ResultColorMax = *ColorsNumMap.Find(DestroyedPrimitiveColor.ToString());
	for (const auto& Color : ColorsNumMap)
	{
		if (ResultColorMax < Color.Value)
		{
			ResultColorMax = Color.Value;
		}
	}
	const FString ColorMaxString = *ColorsNumMap.FindKey(ResultColorMax);
	UE_LOG(LogTemp, Warning, TEXT("ColorMaxString: %s"), *ColorMaxString);
	
	//Find the color that has the least quantity
	int ResultColorMin = *ColorsNumMap.Find(DestroyedPrimitiveColor.ToString());
	for (const auto& Color : ColorsNumMap)
	{
		if (ResultColorMin > Color.Value)
		{
			ResultColorMin = Color.Value;
		}
	}
	const FString ColorMinString = *ColorsNumMap.FindKey(ResultColorMin);
	UE_LOG(LogTemp, Warning, TEXT("ColorMinString: %s"), *ColorMinString);
	
	
	//Need just color adjustment, primitives are good
	bool ColorIsChanged = false;
	if (PrimitiveTypeOverrepresented.IsEmpty() && !PrimitiveColorOverrepresented.IsEmpty())
	{
		for (const auto& Shape : ShapesArray)
		{
			if (Shape->GetPrimitiveColor().ToString().Equals(*ColorMaxString) && !ColorIsChanged)
			{
				const FLinearColor* ColorToChange = ColorsMapString.FindKey(*ColorMinString);
				UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
				UE_LOG(LogTemp, Warning, TEXT("Wytypowany ksztalt: %s, wytypowany kolor do zmiany: %s"), *Shape->GetPrimitiveType().ToString(), *Shape->GetPrimitiveColor().ToString());
				UE_LOG(LogTemp, Warning, TEXT("Zmienieno na kolor: %s"), *ColorMinString);

				ShapeToDelete = Shape;
											
				AddColorToShape(Shape, *ColorToChange);
				ColorIsChanged = true;
				Shape->SetPrimitiveColorInfo(*ColorToChange, ColorsMap);

				ShapeToAdd = Shape;
			}
		}
	}

	//Need just primitiveType adjumstment, colors are good
	bool PrimitiveIsChanged = false;
	if (PrimitiveColorOverrepresented.IsEmpty() && !PrimitiveTypeOverrepresented.IsEmpty())
	{
		for (const auto& Shape : ShapesArray)
		{
			if (Shape->GetPrimitiveType().ToString().Equals(*PrimitiveMaxTypeString) && !PrimitiveIsChanged)
			{
				const TSubclassOf<AShpsBaseShape> ShapeToChange = *PrimitivesMapString.FindKey(*PrimitiveMinTypeString);
				const FLinearColor* ShapeColor = ColorsMapString.FindKey(Shape->GetPrimitiveColor().ToString());

				AShpsBaseShape* NewShape = ChangePrimitiveType(ShapeToChange, Shape);
				NewShape->SetPrimitiveTypeInfo(NewShape->GetClass(), PrimitivesMap);
				NewShape->SetPrimitiveSizeInfo();
												
				AddColorToShape(NewShape, *ShapeColor);
				NewShape->SetPrimitiveColorInfo(*ShapeColor, ColorsMap);

				UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
				UE_LOG(LogTemp, Warning, TEXT("Wytypowany ksztalt: %s, wytypowany kolor do zmiany: %s"), *Shape->GetPrimitiveType().ToString(), *Shape->GetPrimitiveColor().ToString());
				UE_LOG(LogTemp, Warning, TEXT("Zmienieno na ksztalt: %s"), *NewShape->GetPrimitiveType().ToString());

				ShapeToDelete = Shape;
				ShapeToAdd = NewShape;
				Shape->Destroy();
												
				PrimitiveIsChanged = true;
			}
		}
	}
	
	//Need PrimitiveType and Color adjustments
	if (!PrimitiveColorOverrepresented.IsEmpty() && !PrimitiveTypeOverrepresented.IsEmpty())
	{
		for (const auto& Shape : ShapesArray)
		{
			if (Shape->GetPrimitiveType().ToString().Equals(*PrimitiveMaxTypeString) && Shape->GetPrimitiveColor().ToString().Equals(*ColorMaxString) && !PrimitiveIsChanged)
			{
				UE_LOG(LogTemp, Warning, TEXT("Wytypowany ksztalt: %s, wytypowany kolor do zmiany: %s"), *Shape->GetPrimitiveType().ToString(), *Shape->GetPrimitiveColor().ToString());
				
				const TSubclassOf<AShpsBaseShape> ShapeNewType = *PrimitivesMapString.FindKey(*PrimitiveMinTypeString);
				const FLinearColor ShapeNewColor = *ColorsMapString.FindKey(*ColorMinString);

				AShpsBaseShape* NewShape = ChangePrimitiveType(ShapeNewType, Shape);
				NewShape->SetPrimitiveTypeInfo(NewShape->GetClass(), PrimitivesMap);
				NewShape->SetPrimitiveSizeInfo();
												
				AddColorToShape(NewShape, ShapeNewColor);
				NewShape->SetPrimitiveColorInfo(ShapeNewColor, ColorsMap);

				UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
				UE_LOG(LogTemp, Warning, TEXT("Zmienieno na ksztalt: %s, Zmieniono na kolor: %s"), *NewShape->GetPrimitiveType().ToString(), *NewShape->GetPrimitiveColor().ToString());

				ShapeToDelete = Shape;
				ShapeToAdd = NewShape;
				Shape->Destroy();
												
				PrimitiveIsChanged = true;
				
			}
		}
	}
	
	//if (PrimitiveTypeChanged || ColorChanged)
	if (PrimitiveIsChanged || ColorIsChanged)
	{
		ShapesArray.Remove(ShapeToDelete);
		ShapesArray.Add(ShapeToAdd);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
	for (const auto& Shape : ShapesArray)
	{
		UE_LOG(LogTemp, Warning, TEXT("Name: %s, Type: %s, Color:: %s"), *Shape->GetName(), *Shape->GetPrimitiveType().ToString(), *Shape->GetPrimitiveColor().ToString());
	}
	UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));

	PrimitiveColorOverrepresented.Empty();
	PrimitiveTypeOverrepresented.Empty();

	UpdateColorsNumMap(ColorsNumMap);
	UpdatePrimitivesNumMap(PrimitivesNumMap);

	UE_LOG(LogTemp, Warning, TEXT("-----------------------------PO---------------------------------------"));
	for (const auto& Primitives : PrimitivesNumMap)
	{
	UE_LOG(LogTemp, Warning, TEXT("Primitive: %s, Value: %d"), *Primitives.Key, Primitives.Value);
	}
	UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));

	UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
	for (const auto& Colors : ColorsNumMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("Color: %s, Value: %d"), *Colors.Key, Colors.Value);
	}
	UE_LOG(LogTemp, Warning, TEXT("--------------------------------KONIEC----------------------------------------"));
	
}

// Called every frame
void AShpsShapesSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

