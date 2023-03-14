// Fill out your copyright notice in the Description page of Project Settings.


#include "Trigger.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
ATrigger::ATrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Control the difficulty level of matching to music beats.
	TimeErrorMargin = 0.1;
}

// Called when the game starts or when spawned
void ATrigger::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerRef = Cast<ATP_FirstPersonCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	MusicManager = Cast<AMusicManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMusicManager::StaticClass()));
	
	AudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();
	AudioImporter->OnResult.AddDynamic(this, &ATrigger::CheckImportedAudio);
	AudioImporter->ImportAudioFromFile(ImportAudioPath, ERuntimeAudioFormat::Auto);
	

}

// Called every frame
void ATrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATrigger::CheckImportedAudio(URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave,
	ERuntimeImportStatus Status)
{
	
}


