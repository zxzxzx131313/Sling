// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LaunchPedal.h"
#include "SlingHandle.h"
#include "SpeedConveyor.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "ConstantQNRT.h"
#include "Components/StaticMeshComponent.h"
#include "AudioAnalysisTools/Public/AudioAnalysisToolsLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "RuntimeAudioImporter/Public/RuntimeAudioImporterLibrary.h"
#include "TriggerManager.generated.h"

// index for audio analyzer out
#define PEDAL_TRIGGER 24
#define HANDLE_TRIGGER 0
#define CONVEYOR_TRIGGER 47

USTRUCT(BlueprintType)
struct FAudioTimeEntry : public FTableRowBase
{
	GENERATED_BODY()

public:
	FAudioTimeEntry()
	{
		Time = 0.f;
		PedalAudioOn = false;
		HandleAudioOn = false;
		ConveyorAudioOn = false;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PedalAudioOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HandleAudioOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ConveyorAudioOn;
};

USTRUCT(BlueprintType)
struct FDebugeEntry : public FTableRowBase
{
	GENERATED_BODY()

public:
	FDebugeEntry()
	{
		Time = 0.f;
		PedalAudioOn = false;
		HandleAudioOn = false;
		ConveyorAudioOn = false;
		Distance = 0.f;
		
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PedalAudioOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HandleAudioOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ConveyorAudioOn;
};

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
	
	int count;
	
	TArray<float> DistancesToPrevious;

	TArray<float> DistancesToStart;
	
	float TotalSpawnDistance;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ALaunchPedal*> Pedals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ASpeedConveyor*> Conveyors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ASlingHandle*> Handles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAudioComponent *Audio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase *Music;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MusicDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UConstantQNRT *AudioAnalyzer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentPlayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent *DebugPedal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent *DebugHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent *DebugConveyor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent *DebugHiHat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAudioAnalysisToolsLibrary *AudioToolAnalyzer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URuntimeAudioImporterLibrary *AudioImporter;

	// initialized in engine
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ImportAudioPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AStaticMeshActor*> Objs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent *AStaticMesh;

	// initialized in engine
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable *CurrentMusicData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable *DebugMusicData;
	
	// initialized in engine
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MusicDataPath;

	// initialized in engine
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MusicName;
	
public:
	UFUNCTION(BlueprintCallable)
	void AudioPreAnalyze();

	UFUNCTION(BlueprintCallable)
	void CheckImportedAudio(URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave, ERuntimeImportStatus Status);

	// Deprecated: for AudioAnalysisTool
	UFUNCTION(BlueprintCallable)
	void ProcessImportedAudio( const TArray<float>& PCMData);

	// Set the launch Angle of previous pedal, set the spawn distance to previous of current pedal
	UFUNCTION(BlueprintCallable)
	void SetPedalDistanceAngle(int PedalCursor);
	
	UFUNCTION(BlueprintCallable)
	TArray<ATrigger*> SpawnTriggers();
	
	UFUNCTION(BlueprintCallable)
	TArray<float> StrengthsToPedalSpawnDistance();
	
	UFUNCTION(BlueprintCallable)
	void ReactToAudio(const USoundWave* PlayingSoundWave, const float PlaybackPercent);


};
