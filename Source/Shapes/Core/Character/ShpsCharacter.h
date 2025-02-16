// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShpsCharacter.generated.h"

class AShpsBaseShpe;

UCLASS()
class SHAPES_API AShpsCharacter : public ACharacter
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShapeShootedSignaure, AActor*, BaseShapeActor);

public:
	// Sets default values for this character's properties
	AShpsCharacter();

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnShapeShootedSignaure OnShapeShootedDelegate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
