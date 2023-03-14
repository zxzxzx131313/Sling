// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MusicManager.h"

#include "GameFramework/Actor.h"

#include "GameFramework/Character.h"
#include "RuntimeAudioImporter/Public/RuntimeAudioImporterLibrary.h"
#include "TP_FirstPerson/TP_FirstPersonCharacter.h"

#include "Trigger.generated.h"


UCLASS()
class SLING_API ATrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent *Mesh;

	UFUNCTION(BlueprintCallable)
	void CheckImportedAudio(URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave, ERuntimeImportStatus Status);

	// initialized in engine
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ImportAudioPath;
	

	// ATriggerManager *TriggerManager;

	float TimeStamp;

	float TimeErrorMargin;
	
	URuntimeAudioImporterLibrary *AudioImporter;
	
	ATP_FirstPersonCharacter* PlayerRef;

	AMusicManager *MusicManager;
};
