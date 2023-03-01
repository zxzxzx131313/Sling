// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "SlingHandle.generated.h"

UCLASS()
class SLING_API ASlingHandle : public ATrigger
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlingHandle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	
};
