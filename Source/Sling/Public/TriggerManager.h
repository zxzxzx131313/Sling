// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "LaunchPedal.h"
#include "SlingHandle.h"
#include "SpeedConveyor.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "ConstantQNRT.h"
#include "AudioTimeEntry.h"
#include "MusicManager.h"
#include "TriggerManager.generated.h"



// USTRUCT(BlueprintType)
// struct FAudioTimeEntry : public FTableRowBase
// {
// 	GENERATED_BODY()
//
// public:
// 	FAudioTimeEntry()
// 	{
// 		Time = 0.f;
// 		PedalAudioOn = false;
// 		HandleAudioOn = false;
// 		ConveyorAudioOn = false;
// 	}
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float Time;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	bool PedalAudioOn;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	bool HandleAudioOn;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	bool ConveyorAudioOn;
// };


UCLASS()
class SLING_API ATriggerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriggerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	TArray<FAudioTimeEntry*> TriggerHits;

	TArray<ATrigger*> SpawnedTriggers;
	
	TArray<float> DistancesToPrevious;

	TArray<float> DistancesToStart;
	
	float TotalSpawnDistance;

	AMusicManager *MusicManager;

	ATP_FirstPersonCharacter* PlayerRef;

public:
	// initialized in engine
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable *CurrentMusicData;

	// initialized in engine
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MusicName;

	
public:
	UFUNCTION(BlueprintCallable)
	void FillMusicDataTable();
	
	// Set the launch Angle of previous pedal, set the spawn distance to previous of current pedal
	UFUNCTION(BlueprintCallable)
	void SetPedalDistanceAngle(int PedalCursor);
	
	UFUNCTION(BlueprintCallable)
	void SpawnTriggers();
	

};
