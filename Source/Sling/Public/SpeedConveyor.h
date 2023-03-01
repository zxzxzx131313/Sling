// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trigger.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "SpeedConveyor.generated.h"

UCLASS()
class SLING_API ASpeedConveyor : public ATrigger
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpeedConveyor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent *Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent *CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaunchSpeed;

private:
	bool Overlapping;

	FVector Force;
};
