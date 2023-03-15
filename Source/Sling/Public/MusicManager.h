// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioAnalysisToolsLibrary.h"
#include "ConstantQNRT.h"
#include "RuntimeAudioImporterLibrary.h"
#include "Engine/DataTable.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "AudioTimeEntry.h"

#include "MusicManager.generated.h"

// index for audio analyzer out
#define PEDAL_TRIGGER 24
#define HANDLE_TRIGGER 0
#define CONVEYOR_TRIGGER 47


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompleteAudioAnalyze);

UCLASS()
class SLING_API AMusicManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMusicManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void AudioPreAnalyze();

	UFUNCTION(BlueprintCallable)
	void CheckImportedAudio(URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave, ERuntimeImportStatus Status);

	UFUNCTION(BlueprintCallable)
	float GetMusicPlayTime();

	UFUNCTION(BlueprintCallable)
	void ReactToAudio(const USoundWave* PlayingSoundWave, const float PlaybackPercent);

public:

	UPROPERTY(BlueprintReadOnly)
	UDataTable *CurrentMusicData;
	
	// initialized in engine
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MusicName;
	
	UPROPERTY(BlueprintReadOnly)
	UAudioComponent *Audio;

	UPROPERTY(BlueprintReadOnly)
	USoundBase *Music;

	UPROPERTY(BlueprintReadOnly)
	float MusicDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UConstantQNRT *AudioAnalyzer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent *DebugPedal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent *DebugHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent *DebugConveyor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent *DebugHiHat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URuntimeAudioImporterLibrary *AudioImporter;

	// initialized in engine
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ImportAudioPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AStaticMeshActor*> Objs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent *AStaticMesh;

	UPROPERTY(BlueprintAssignable)
	FOnCompleteAudioAnalyze OnCompleteAudioAnalyze;
	
private:
	
	float CurrentPlayTime;
	
	int Count;

	TArray<FAudioTimeEntry*> TriggerHits;


};
