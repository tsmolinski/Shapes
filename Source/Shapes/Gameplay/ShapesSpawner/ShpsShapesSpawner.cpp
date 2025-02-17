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
	UpdatePrimitivesNumMap();
	UpdateColorsNumMap();
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

bool AShpsShapesSpawner::SameNumberOfEachPrimitive(TMap<FString, int> PrimitivesNum)
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

bool AShpsShapesSpawner::SameNumberOfEachColor(TMap<FString, int> ColorsNum)
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
	UpdatePrimitivesNumMap();
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
	UpdateColorsNumMap();
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

void AShpsShapesSpawner::UpdatePrimitivesNumMap()
{
	for (auto& Primitive : PrimitivesMap)
	{
		int Index = 0;
		for (auto& Shape : ShapesArray)
		{
			if (Primitive.Value.EqualTo(Shape->GetPrimitiveType()))
			{
				++Index;
				PrimitivesNumMap.Add(Shape->GetPrimitiveType().ToString(), Index);
			}
		}
	}
}
UE_ENABLE_OPTIMIZATION

void AShpsShapesSpawner::UpdateColorsNumMap()
{
	for (auto& Color : ColorsMap)
	{
		int Index = 0;
		for (auto& Shape : ShapesArray)
		{
			if (Color.Value.EqualTo(Shape->GetPrimitiveColor()))
			{
				++Index;
				ColorsNumMap.Add(Shape->GetPrimitiveColor().ToString(), Index);
			}
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

	UpdateColorsNumMap();
	UpdatePrimitivesNumMap();

	TArray<FString> PrimitiveTypeOverrepresented = PrimitivesTypeAboveToleranceNumber(PrimitivesNumMap, DestroyedPrimitiveType);
	TArray<FString> PrimitiveColorOverrepresented = ColorsAboveToleranceNumber(ColorsNumMap, DestroyedPrimitiveColor);
	
	TObjectPtr<AShpsBaseShape> ShapeToDelete;
	TObjectPtr<AShpsBaseShape> ShapeToAdd;
	
	//Need just color adjustment, primitives are good
	bool ColorChanged = false;
	if (PrimitiveTypeOverrepresented.IsEmpty() && !PrimitiveColorOverrepresented.IsEmpty())
	{
		//Update required, otherwise there are different numbers, haven't found the reason for this bug.
		UpdatePrimitivesNumMap();
		bool PrimitivesAreEqual = SameNumberOfEachPrimitive(PrimitivesNumMap);
		int* result = PrimitivesNumMap.Find(DestroyedPrimitiveType.ToString());
			for (const auto& Primitive : PrimitivesNumMap)
			{
				//Find the Primitive type that has the most or choose the first one if there are the same number of Primitive types.
				if (Primitive.Value > *result || PrimitivesAreEqual)
				{
					PrimitivesAreEqual = !PrimitivesAreEqual;
					*result = Primitive.Value;
					UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
					UE_LOG(LogTemp, Warning, TEXT("Ksztalt, ktorego jest najwiecej: %s"), *Primitive.Key);

					//Iterate through the shapes to find the one that has the right Primitive type (defined earlier) and color
					for (const auto& Shape : ShapesArray)
					{
						if (Shape->GetPrimitiveType().ToString().Equals(Primitive.Key) && !ColorChanged)
						{
							for (const auto& PrimitiveColor : PrimitiveColorOverrepresented)
							{
								if (Shape->GetPrimitiveColor().ToString().Equals(PrimitiveColor))
								{
									UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
									UE_LOG(LogTemp, Warning, TEXT("Wytypowany ksztalt: %s, wytypowany kolor: %s"), *Shape->GetPrimitiveType().ToString(), *Shape->GetPrimitiveColor().ToString());
									
									UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
									UE_LOG(LogTemp, Warning, TEXT("Kasowanie koloru z konkretnego ksztaltu"));
									UE_LOG(LogTemp, Warning, TEXT("Color Name: %s, Color.Value: %d"), *Shape->GetPrimitiveColor().ToString(), (*ColorsNumMap.Find(Shape->GetPrimitiveColor().ToString()) - 1));
									UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));

									//Find FLinearColor in ColorsMap for Color change/swap
									for (const auto& Color : ColorsMap)
									{
										if (!PrimitiveColorOverrepresented.Contains(Color.Value.ToString()) && !ColorChanged)
										{
											ShapeToDelete = Shape;
											
											AddColorToShape(Shape, Color.Key);
											ColorChanged = true;
											Shape->SetPrimitiveColorInfo(Color.Key, ColorsMap);

											ShapeToAdd = Shape;
											
											UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
											UE_LOG(LogTemp, Warning, TEXT("Zmieniam konkretny ksztalt na brakujacy kolor"));
											UE_LOG(LogTemp, Warning, TEXT("Color Name: %s, Color.Value: %d"), *Shape->GetPrimitiveColor().ToString(), (*ColorsNumMap.Find(Shape->GetPrimitiveColor().ToString()) + 1));
											UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
										}
									}
								}
							}
						}
					}
				}
			}
	}

	//Need just primitiveType adjumstment, colors are good
	bool PrimitiveTypeChanged = false;
	if (PrimitiveColorOverrepresented.IsEmpty() && !PrimitiveTypeOverrepresented.IsEmpty())
	{
		//Update required, otherwise there are different numbers, haven't found the reason for this bug.
		UpdateColorsNumMap();
		bool ColorsAreEqual = SameNumberOfEachColor(ColorsNumMap);
		int* result = ColorsNumMap.Find(DestroyedPrimitiveColor.ToString());
		for (const auto& ColorNum : ColorsNumMap)
		{
			//Find the color that has the most or choose the first one if there are the same number of colors.
			if (ColorNum.Value > *result || ColorsAreEqual)
			{
				ColorsAreEqual = !ColorsAreEqual;
				*result = ColorNum.Value;
				UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
				UE_LOG(LogTemp, Warning, TEXT("Kolor, ktorego jest najwiecej: %s"), *ColorNum.Key);

				//Iterate through the shapes to find the one that has the right color (defined earlier) and primitive type
				for (const auto& Shape : ShapesArray)
				{
					if (Shape->GetPrimitiveColor().ToString().Equals(ColorNum.Key) && !PrimitiveTypeChanged)
					{
						for (const auto& PrimitiveType : PrimitiveTypeOverrepresented)
						{
							if (Shape->GetPrimitiveType().ToString().Equals(PrimitiveType))
							{
								UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
								UE_LOG(LogTemp, Warning, TEXT("Wytypowany ksztalt: %s, wytypowany kolor: %s"), *Shape->GetPrimitiveType().ToString(), *Shape->GetPrimitiveColor().ToString());
								
								UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
								UE_LOG(LogTemp, Warning, TEXT("Kasowanie ksztaltu z konkretnego koloru"));
								UE_LOG(LogTemp, Warning, TEXT("PrimitiveType Name: %s, PrimitivesNumMap.Value: %d"), *Shape->GetPrimitiveType().ToString(), (*PrimitivesNumMap.Find(Shape->GetPrimitiveType().ToString()) - 1));
								UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));

								//Update required, otherwise there are different numbers, haven't found the reason for this bug.
								UpdatePrimitivesNumMap();
								//Find Primitive Type that is not overrepresented, because we need more of them.
								for (const auto& Primitive : PrimitivesMap)
								{
									if (!PrimitiveTypeOverrepresented.Contains(Primitive.Value.ToString()) && !PrimitiveTypeChanged)
									{
										//Update required, otherwise there are different numbers, haven't found the reason for this bug.
										UpdatePrimitivesNumMap();
										//Check the least number of not overrepresented primitives.
										int* SmallestPrimitiveNum = PrimitivesNumMap.Find(Shape->GetPrimitiveType().ToString());
										UE_LOG(LogTemp, Warning, TEXT("SMALLEST PRIMITIVE NUM PRZED: %d"), *SmallestPrimitiveNum);

										for (const auto& PrimitiveNum2 : PrimitivesNumMap)
										{
											UE_LOG(LogTemp, Warning, TEXT("SMALLEST PRIMITIVE NUM2: %d"), PrimitiveNum2.Value);
											if (PrimitiveNum2.Value < *SmallestPrimitiveNum)
											{
												*SmallestPrimitiveNum = PrimitiveNum2.Value;
												UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
												UE_LOG(LogTemp, Warning, TEXT("Ksztalt, ktorego jest najmniej: %s"), *PrimitiveNum2.Key);
												UE_LOG(LogTemp, Warning, TEXT("SMALLEST PRIMITIVE NUM KONIEC: %d"), *SmallestPrimitiveNum);

												//Find Primitive TSubclassOf for spawning
												for (const auto& Primitive2 : PrimitivesMap)
												{
													//Find the Primitive type that has the least amount for spawning
													if (Primitive2.Value.ToString().Contains(PrimitiveNum2.Key))
													{
														UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
														UE_LOG(LogTemp, Warning, TEXT("Ksztalt, ktorego jest najmniej----------------------------------------: %s"), *Primitive2.Value.ToString());

														//Find FLinearColor in ColorsMap for spawning
														for (const auto& Color : ColorsMap)
														{
															//Find FLinearColor in ColorsMap for spawning
															if (Color.Value.ToString().Contains((Shape->GetPrimitiveColor().ToString())))
															{
																AShpsBaseShape* NewShape = ChangePrimitiveType(Primitive2.Key, Shape);
																NewShape->SetPrimitiveTypeInfo(NewShape->GetClass(), PrimitivesMap);
																NewShape->SetPrimitiveSizeInfo();
												
																AddColorToShape(NewShape, Color.Key);
																NewShape->SetPrimitiveColorInfo(Color.Key, ColorsMap);

																ShapeToDelete = Shape;
																ShapeToAdd = NewShape;
																Shape->Destroy();
												
																PrimitiveTypeChanged = true;

																UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
																UE_LOG(LogTemp, Warning, TEXT("Zmieniam konkretny kolor na brakujacy ksztalt"));
																UE_LOG(LogTemp, Warning, TEXT("PrimitiveType Name: %s, PrimitivesNumMap.Value: %d"), *NewShape->GetPrimitiveType().ToString(), (*PrimitivesNumMap.Find(NewShape->GetPrimitiveType().ToString()) + 1));
																UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	//Need primitive type and color adjustment
	if (!PrimitiveColorOverrepresented.IsEmpty() && !PrimitiveTypeOverrepresented.IsEmpty())
	{
		//Update required, otherwise there are different numbers, haven't found the reason for this bug.
		UpdateColorsNumMap();
		int* result = ColorsNumMap.Find(DestroyedPrimitiveColor.ToString());
		for (const auto& ColorNum : ColorsNumMap)
		{
			//Find the Color that has the more or choose the first one if there are the same number of Colors.
			if (ColorNum.Value > *result && !PrimitiveTypeChanged)
			{
				*result = ColorNum.Value;
				UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
				UE_LOG(LogTemp, Warning, TEXT("Kolor, ktorego jest najwiecej: %s"), *ColorNum.Key);

				//Iterate through the shapes to find the one that has the right color (defined earlier) and primitive type
				for (const auto& Shape : ShapesArray)
				{
					if (Shape->GetPrimitiveColor().ToString().Equals(ColorNum.Key) && !PrimitiveTypeChanged)
					{
						for (const auto& PrimitiveType : PrimitiveTypeOverrepresented)
						{
							if (Shape->GetPrimitiveType().ToString().Equals(PrimitiveType))
							{
								UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
								UE_LOG(LogTemp, Warning, TEXT("Wytypowany ksztalt: %s, wytypowany kolor: %s"), *Shape->GetPrimitiveType().ToString(), *Shape->GetPrimitiveColor().ToString());
								
								UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
								UE_LOG(LogTemp, Warning, TEXT("Kasowanie ksztaltu z konkretnego koloru"));
								UE_LOG(LogTemp, Warning, TEXT("PrimitiveType Name: %s, PrimitivesNumMap.Value: %d"), *Shape->GetPrimitiveType().ToString(), (*PrimitivesNumMap.Find(Shape->GetPrimitiveType().ToString()) - 1));
								UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));

								for (const auto& Primitive : PrimitivesMap)
								{
									if (!PrimitiveTypeOverrepresented.Contains(Primitive.Value.ToString()) && !PrimitiveTypeChanged)
									{
										//Update required, otherwise there are different numbers, haven't found the reason for this bug.
										UpdateColorsNumMap();
										//Check the least number of not overrepresented primitives that has right color
										int* SmallestColorNum = ColorsNumMap.Find(Shape->GetPrimitiveColor().ToString());
										UE_LOG(LogTemp, Warning, TEXT("SMALLEST COLOR NUM PRZED: %d"), *SmallestColorNum);
										
										for (const auto& ColorNum2 : ColorsNumMap)
										{
											UE_LOG(LogTemp, Warning, TEXT("COLORNUM2.VALUE: %d"), ColorNum2.Value);
											if (ColorNum2.Value < *SmallestColorNum)
											{
												*SmallestColorNum = ColorNum2.Value;
												UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
												UE_LOG(LogTemp, Warning, TEXT("Kolor, ktorego jest najmniej: %s"), *ColorNum2.Key);
												UE_LOG(LogTemp, Warning, TEXT("SMALLEST COLOR NUM KONIEC: %d"), *SmallestColorNum);

												//Find FLinearColor for spawning
												for (const auto& Color : ColorsMap)
												{
													if (Color.Value.ToString().Contains(ColorNum2.Key))
													{
														AShpsBaseShape* NewShape = ChangePrimitiveType(Primitive.Key, Shape);
														NewShape->SetPrimitiveTypeInfo(NewShape->GetClass(), PrimitivesMap);
														NewShape->SetPrimitiveSizeInfo();
												
														AddColorToShape(NewShape, Color.Key);
														NewShape->SetPrimitiveColorInfo(Color.Key, ColorsMap);

														ShapeToDelete = Shape;
														ShapeToAdd = NewShape;
														Shape->Destroy();
												
														PrimitiveTypeChanged = true;

														UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
														UE_LOG(LogTemp, Warning, TEXT("Zmieniam konkretny kolor na brakujacy ksztalt"));
														UE_LOG(LogTemp, Warning, TEXT("PrimitiveType Name: %s, PrimitivesNumMap.Value: %d"), *NewShape->GetPrimitiveType().ToString(), (*PrimitivesNumMap.Find(NewShape->GetPrimitiveType().ToString()) + 1));
														UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
													}
												}
											}
										}
										
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	
	if (PrimitiveTypeChanged || ColorChanged)
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

	UpdateColorsNumMap();
	UpdatePrimitivesNumMap();

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

