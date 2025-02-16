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
		
		//FVector ShapeLocation = Shape->GetActorLocation();
		//FVector ShapeSize = Shape->GetActorScale();
		
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
				//TObjectPtr<UMaterialInstanceDynamic> MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(Material, Shape);
				Shape->ShapeMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(Material, Shape);
				TObjectPtr<UMaterialInstanceDynamic> MaterialInstanceDynamic = Shape->ShapeMaterialInstanceDynamic;
				
				if (MaterialInstanceDynamic)
				{
					ShapeMeshComponent->SetMaterial(0, MaterialInstanceDynamic);
					
					int ColorsArrayIndex = Index % ColorsArray.Num();
					MaterialInstanceDynamic->SetVectorParameterValue(FName("Color"), ColorsArray[ColorsArrayIndex]);
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

	//for (auto& Primitive : PrimitivesNumMap)
	//{
		//UE_LOG(LogTemp, Warning, TEXT("Primitives Num Map: Primitive: %s  Amount: %d"), *Primitive.Key, Primitive.Value);
	//}
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

	//for (auto& Color : ColorsNumMap)
	//{
		//UE_LOG(LogTemp, Warning, TEXT("Colors Num Map: Color: %s  Amount: %d"), *Color.Key, Color.Value);
	//}
}

void AShpsShapesSpawner::OnShapeShooted(AActor* BaseShapeActor)
{
	UE_LOG(LogTemp, Warning, TEXT("The AShpsShapesSpawner::OnShapeShooted: %s"), *BaseShapeActor->GetName());

	TObjectPtr<AShpsBaseShape> DestroyedBaseShape = Cast<AShpsBaseShape>(BaseShapeActor);
	FText DestroyedPrimitiveType = DestroyedBaseShape->GetPrimitiveType();
	//UE_LOG(LogTemp, Warning, TEXT("Destroyed Primitive Type: %s"), *DestroyedPrimitiveType.ToString());
	FText DestroyedPrimitiveColor = DestroyedBaseShape->GetPrimitiveColor();
	//UE_LOG(LogTemp, Warning, TEXT("Destroyed Primitive Color: %s"), *DestroyedPrimitiveColor.ToString());

	TSubclassOf<AShpsBaseShape> DestroyedShapeClass = DestroyedBaseShape->GetClass();
	
	ShapesArray.Remove(DestroyedBaseShape);
	DestroyedBaseShape->Destroy();

	UpdateColorsNumMap();
	UpdatePrimitivesNumMap();

	TArray<FString> PrimitiveTypeOverrepresented;
	int* TempPrimitiveNum = PrimitivesNumMap.Find(DestroyedPrimitiveType.ToString());
	//PrimitivesNumMap.Add(DestroyedPrimitiveType.ToString(), *TempPrimitiveNum - 1);

	TArray<FString> PrimitiveColorOverrepresented;
	int* TempColorsNum = ColorsNumMap.Find(DestroyedPrimitiveColor.ToString());
	//ColorsNumMap.Add(DestroyedPrimitiveColor.ToString(), *tempColorsNum - 1);
	
	TObjectPtr<AShpsBaseShape> ShapeToDelete;
	TObjectPtr<AShpsBaseShape> ShapeToAdd;

	for (const auto& PrimitiveNum : PrimitivesNumMap)
	{
		//if (!PrimitiveNum.Key.Equals(DestroyedPrimitiveType.ToString()))
		//{
			//if (abs((PrimitiveNum.Value - (*TempPrimitiveNum - 1))) > ToleranceNumber)
			if (abs((PrimitiveNum.Value - (*TempPrimitiveNum))) > ToleranceNumber)
			{
				//INACZEJ
				// sprawdzamy, tak jak tutaj, ktorych jest za duzo i wrzucamy ich do nowej listy
				// pozniej bierzemy wszystkiech z nich (petla), sprawdzamy mape kolorow - jesli w zadnym roznica nie jest wieksza niz jeden, to bierzemy pierwszy primitive, obojetnie jaki kolor i go kasujemy
				// tworzymy nowy Shape z tym samym kolore, co zniszczylismy.
				// jesli w mapie kolorow wyjdzie, ze tylko w jednym z nich jest nadpodaz kolorow - to go usuwamy, niewazne, czy to pierwszy czy kolejny na liscie
				
				PrimitiveTypeOverrepresented.Add(PrimitiveNum.Key);
				UE_LOG(LogTemp, Warning, TEXT("Za duzo Primitives: %s"), *PrimitiveNum.Key);
			}
		//}
	}
	//PrimitivesNumMap.Add(DestroyedPrimitiveType.ToString(), *TempPrimitiveNum - 1);
	
	for (const auto& ColorNum : ColorsNumMap)
	{
		//if (!ColorNum.Key.Equals(DestroyedPrimitiveColor.ToString()))
		//{
			//if (abs(ColorNum.Value - (*TempColorsNum - 1) > ToleranceNumber))
			if (abs(ColorNum.Value - (*TempColorsNum) > ToleranceNumber))
			{
				PrimitiveColorOverrepresented.Add(ColorNum.Key);
				UE_LOG(LogTemp, Warning, TEXT("Za duzo Koloru: %s"), *ColorNum.Key);
			}
		//}
	}
	//ColorsNumMap.Add(DestroyedPrimitiveColor.ToString(), *TempColorsNum - 1);
	
	//Need just color adjustment, primitives are good
	bool ColorChanged = false;
	if (PrimitiveTypeOverrepresented.IsEmpty() && !PrimitiveColorOverrepresented.IsEmpty())
	{
		UpdatePrimitivesNumMap();
		int* result = PrimitivesNumMap.Find(DestroyedPrimitiveType.ToString());
			for (const auto& Primitive : PrimitivesNumMap)
			{
				if (Primitive.Value > *result)
				{
					*result = Primitive.Value;
					UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
					UE_LOG(LogTemp, Warning, TEXT("Ksztalt, ktorego jest najwiecej: %s"), *Primitive.Key);

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
									
									//ColorsNumMap.Add(Shape->GetPrimitiveColor().ToString(), *ColorsNumMap.Find(Shape->GetPrimitiveColor().ToString()) - 1);
									UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
									UE_LOG(LogTemp, Warning, TEXT("Kasowanie koloru z konkretnego ksztaltu"));
									UE_LOG(LogTemp, Warning, TEXT("Color Name: %s, Color.Value: %d"), *Shape->GetPrimitiveColor().ToString(), (*ColorsNumMap.Find(Shape->GetPrimitiveColor().ToString()) - 1));
									UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
									
									for (const auto& Color : ColorsMap)
									{
										if (!PrimitiveColorOverrepresented.Contains(Color.Value.ToString()))
										{
											//ShapesArray.Remove(Shape);
											
											ShapeToDelete = Shape;
											
											AddColorToShape(Shape, Color.Key);
											//ShapesArray.Add(Shape);
											ColorChanged = true;
											Shape->SetPrimitiveColorInfo(Color.Key, ColorsMap);

											ShapeToAdd = Shape;
											
											//ColorsNumMap.Add(Shape->GetPrimitiveColor().ToString(), *ColorsNumMap.Find(Shape->GetPrimitiveColor().ToString()) + 1);
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
	//if (!PrimitiveTypeOverrepresented.IsEmpty())
	{
		UpdateColorsNumMap();
		int* result = ColorsNumMap.Find(DestroyedPrimitiveColor.ToString());
		for (const auto& ColorNum : ColorsNumMap)
		{
			if (ColorNum.Value > *result)
			{
				*result = ColorNum.Value;
				UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
				UE_LOG(LogTemp, Warning, TEXT("Kolor, ktorego jest najwiecej: %s"), *ColorNum.Key);

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

								//PrimitivesNumMap.Add(Shape->GetPrimitiveType().ToString(), *PrimitivesNumMap.Find(Shape->GetPrimitiveType().ToString()) - 1);
								UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
								UE_LOG(LogTemp, Warning, TEXT("Kasowanie ksztaltu z konkretnego koloru"));
								UE_LOG(LogTemp, Warning, TEXT("PrimitiveType Name: %s, PrimitivesNumMap.Value: %d"), *Shape->GetPrimitiveType().ToString(), (*PrimitivesNumMap.Find(Shape->GetPrimitiveType().ToString()) - 1));
								UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
								
								//FVector ShapeLocation = Shape->GetActorLocation();
								//FVector ShapeSize = Shape->GetActorScale();

								UpdatePrimitivesNumMap();
								for (const auto& Primitive : PrimitivesMap)
								{
									if (!PrimitiveTypeOverrepresented.Contains(Primitive.Value.ToString()))
									{
										//AShpsBaseShape* NewShape = ChangePrimitiveType(Primitive.Key, Shape);
										//NewShape->SetPrimitiveTypeInfo(NewShape->GetClass(), PrimitivesMap);
										//NewShape->SetPrimitiveSizeInfo();
										for (const auto& Color : ColorsMap)
										{
											//if (Color.Value.ToString().Equals(Shape->GetPrimitiveColor().ToString()))
											if (Color.Value.ToString().Contains((Shape->GetPrimitiveColor().ToString())))
											{
												//ShapesArray.Remove(Shape);
												AShpsBaseShape* NewShape = ChangePrimitiveType(Primitive.Key, Shape);
												NewShape->SetPrimitiveTypeInfo(NewShape->GetClass(), PrimitivesMap);
												NewShape->SetPrimitiveSizeInfo();
												
												AddColorToShape(NewShape, Color.Key);
												NewShape->SetPrimitiveColorInfo(Color.Key, ColorsMap);

												//PrimitivesNumMap.Add(NewShape->GetPrimitiveType().ToString(), *PrimitivesNumMap.Find(NewShape->GetPrimitiveType().ToString()) + 1);

												ShapeToDelete = Shape;
												ShapeToAdd = NewShape;
												Shape->Destroy();
												
												//ShapesArray.Add(NewShape);
												
												PrimitiveTypeChanged = true;

												UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
												UE_LOG(LogTemp, Warning, TEXT("Zmieniam konkretny kolor na brakujacy ksztalt"));
												UE_LOG(LogTemp, Warning, TEXT("PrimitiveType Name: %s, PrimitivesNumMap.Value: %d"), *NewShape->GetPrimitiveType().ToString(), (*PrimitivesNumMap.Find(NewShape->GetPrimitiveType().ToString()) + 1));
												UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
											}
										}
										//AddColorToShape(NewShape, *ColorsMap.FindKey(Shape->GetColor()));
										//NewShape->SetPrimitiveColorInfo(*ColorsMap.FindKey(Shape->GetColor()), ColorsMap);

										//PrimitivesNumMap.Add(NewShape->GetPrimitiveType().ToString(), *PrimitivesNumMap.Find(NewShape->GetPrimitiveType().ToString()) + 1);
										//PrimitiveTypeChanged = true;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("-----------------------------PRZED------------------------------------------"));
	//for (const auto& Primitives : PrimitivesNumMap)
	//{
		//UE_LOG(LogTemp, Warning, TEXT("Primitive: %s, Value: %d"), *Primitives.Key, Primitives.Value);
	//}
	//UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));

	//UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
	//for (const auto& Colors : ColorsNumMap)
	//{
		//UE_LOG(LogTemp, Warning, TEXT("Color: %s, Value: %d"), *Colors.Key, Colors.Value);
	//}
	//UE_LOG(LogTemp, Warning, TEXT("--------------------------KONIEC-----------------------------------------"));

	//Need primitive type and color adjustment
	if (!PrimitiveColorOverrepresented.IsEmpty() && !PrimitiveTypeOverrepresented.IsEmpty())
	{
		UpdateColorsNumMap();
		int* result = ColorsNumMap.Find(DestroyedPrimitiveColor.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("RESULT PRZED: %d"), *result);
		//UE_LOG(LogTemp, Warning, TEXT("DestroyedPrimitiveColor.ToString(): %s"), *DestroyedPrimitiveColor.ToString());
		for (const auto& ColorNum : ColorsNumMap)
		{
			//UE_LOG(LogTemp, Warning, TEXT("COLORNUM.VALUE: %d"), ColorNum.Value);
			if (ColorNum.Value > *result)
			{
				*result = ColorNum.Value;
				UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
				UE_LOG(LogTemp, Warning, TEXT("Kolor, ktorego jest najwiecej: %s"), *ColorNum.Key);
				//UE_LOG(LogTemp, Warning, TEXT("RESULT KONIEC: %d"), *result);

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

								//PrimitivesNumMap.Add(Shape->GetPrimitiveType().ToString(), *PrimitivesNumMap.Find(Shape->GetPrimitiveType().ToString()) - 1);
								UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));
								UE_LOG(LogTemp, Warning, TEXT("Kasowanie ksztaltu z konkretnego koloru"));
								UE_LOG(LogTemp, Warning, TEXT("PrimitiveType Name: %s, PrimitivesNumMap.Value: %d"), *Shape->GetPrimitiveType().ToString(), (*PrimitivesNumMap.Find(Shape->GetPrimitiveType().ToString()) - 1));
								UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"));

								for (const auto& Primitive : PrimitivesMap)
								{
									if (!PrimitiveTypeOverrepresented.Contains(Primitive.Value.ToString()))
									{
										// TO DLA TESTU ---------------------------------------------------------------------------
										UpdateColorsNumMap();
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
												
												for (const auto& Color : ColorsMap)
												{
													//if (Color.Value.ToString().Contains((Shape->GetPrimitiveColor().ToString())))
													if (Color.Value.ToString().Contains(ColorNum2.Key))
													{
														AShpsBaseShape* NewShape = ChangePrimitiveType(Primitive.Key, Shape);
														NewShape->SetPrimitiveTypeInfo(NewShape->GetClass(), PrimitivesMap);
														NewShape->SetPrimitiveSizeInfo();
												
														AddColorToShape(NewShape, Color.Key);
														NewShape->SetPrimitiveColorInfo(Color.Key, ColorsMap);

														//PrimitivesNumMap.Add(NewShape->GetPrimitiveType().ToString(), *PrimitivesNumMap.Find(NewShape->GetPrimitiveType().ToString()) + 1);

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
	

	//Zmienic kolor i ksztalt, np. niszczac ten sam ksztalt tego samego koloru
	//Jak rozwiazac: to samo co w zmianie ksztaltu, wtedy robi dobry ksztalt, tylko zlyu kolor - zamiast ustawiac ten, ktorego jest najwiecej, to znalezc ten, ktorego jest najmniej
	// i ten ustawic.

	// CO Z SHAPESARRAY? CZY NIE TRZEBA JEJ NA KONCU ZAKTUALIZOWAC? -> usuwamy shape, ktory zniszczylismy
	// ale czy nie powinnismy usunac ten zmieniony shape (typ albo kolor) i dodac nowy (typ albo kolor)?
	// na koncu najlepiej byloby to pozmieniac, jak juz zrobimy change color albo change shape

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

