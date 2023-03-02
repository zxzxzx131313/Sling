// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "LaunchPedal.generated.h"

UCLASS()
class SLING_API ALaunchPedal : public ATrigger
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaunchPedal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FVector ComputeLaunchForce();

	// UFUNCTION(BlueprintCallable)
	// float ComputeTimeInterval(FRotator Angle, float Speed);
	//
	UFUNCTION(BlueprintCallable)
	float ComputeDistance(float Angle, float Speed, float HeightDiff);

	// Compute the angle of a projectile given in-air time interval and initial speed.
	UFUNCTION(BlueprintCallable)
	float ComputeAngle(float TimeInterval, float Speed);

	UFUNCTION(BlueprintCallable)
	FQuat TransformAngleToWorldSpace(float Angle);

	// Set the launch angle rotator in world space. Default launch angle is in direction of FVector(0,1,0);
	UFUNCTION(BlueprintCallable)
	void SetAngle(FRotator Angle);

	UFUNCTION(BlueprintCallable)
	void SetSpeed(float Speed);

	UFUNCTION(BlueprintCallable)
	void SetFlyDistance(float Distance);

	UFUNCTION(BlueprintCallable)
	float GetFlyDistance();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent *Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent *CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaunchDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaunchMAXHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaunchTimeInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Gravity;

private:
	float LaunchSpeed;
	
	FRotator LaunchAngle;

	float FlyDistance;
};
