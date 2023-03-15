// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicManager.h"

#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMusicManager::AMusicManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	Audio->SetupAttachment(RootComponent);
	Audio->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinderOptional<UConstantQNRT> AnalyzerFinder(TEXT("/Script/AudioSynesthesia.ConstantQNRT'/Game/Audio/SN_SytnesthesiaNRT.SN_SytnesthesiaNRT'"));
	if (AnalyzerFinder.Succeeded())
	{
		AudioAnalyzer = AnalyzerFinder.Get();
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot Find analyzer."));
	}

	
	DebugHandle = CreateDefaultSubobject<UStaticMeshComponent>("DebugHandle");
	DebugConveyor = CreateDefaultSubobject<UStaticMeshComponent>("DebugConveyor");
	DebugPedal = CreateDefaultSubobject<UStaticMeshComponent>("DebugPedal");
	DebugHiHat = CreateDefaultSubobject<UStaticMeshComponent>("DebugHiHat");

	DebugHandle->SetupAttachment(RootComponent);
	DebugConveyor->SetupAttachment(RootComponent);
	DebugPedal->SetupAttachment(RootComponent);
	DebugHiHat->SetupAttachment(RootComponent);

	DebugConveyor->SetRelativeLocation(FVector(0 ,200, 0));
	DebugHandle->SetRelativeLocation(FVector(0 ,0, 0));
	DebugPedal->SetRelativeLocation(FVector(0 ,-200, 0));

	AStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("TestMesh");
	AStaticMesh->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AMusicManager::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentMusicData = NewObject<UDataTable>(this, UDataTable::StaticClass(), TEXT("CurrentMusicData"));
	CurrentMusicData->RowStruct = FAudioTimeEntry::StaticStruct();
	
	DebugHiHat->SetWorldLocation(FVector(0 ,0, 80));
	// Import Audio from file
	// AudioToolAnalyzer = UAudioAnalysisToolsLibrary::CreateAudioAnalysisTools(4096, EAnalysisWindowType::HammingWindow);
	AudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();
	AudioImporter->OnResult.AddDynamic(this, &AMusicManager::CheckImportedAudio);
	AudioImporter->ImportAudioFromFile(ImportAudioPath, ERuntimeAudioFormat::Auto);
	Audio->OnAudioPlaybackPercent.AddDynamic(this, &AMusicManager::ReactToAudio);

	Count = 0;
	AStaticMeshActor *spawned = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
	spawned->SetMobility(EComponentMobility::Movable);
	spawned->SetActorLocation(FVector(0,0,80));
	spawned->GetStaticMeshComponent()->SetStaticMesh(AStaticMesh->GetStaticMesh());
	Objs.Add(spawned);

	 
}

// Called every frame
void AMusicManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMusicManager::AudioPreAnalyze()
{
	FString CSVPath = FPaths::ProjectDir()+TEXT("Content/MusicData/")+MusicName.ToString()+TEXT(".csv");
	if(FPlatformFileManager::Get().GetPlatformFile().FileExists(*FPaths::ConvertRelativePathToFull(CSVPath)))
	{
		UDataTableFunctionLibrary::FillDataTableFromCSVFile(CurrentMusicData, CSVPath);
		CurrentMusicData->GetAllRows(TEXT("MusicDataTable"), TriggerHits);
	}
	else
	{
		int TimePrecision = FMath::FloorToInt(100.f * MusicDuration);
		FAudioTimeEntry Entry;
     	for (int i = 0; i < TimePrecision; i++)
     	{
     		TArray<float> StrengthsPerMilliseconds;
     		float TimeCursor = i / 100.f;
     		AudioAnalyzer->GetNormalizedChannelConstantQAtTime(TimeCursor, 0, StrengthsPerMilliseconds);
     		bool PedalHit = StrengthsPerMilliseconds[PEDAL_TRIGGER] > 0.65f;
     		bool HandleHit = StrengthsPerMilliseconds[HANDLE_TRIGGER] > 0.6f;
     		bool ConveyorHit = StrengthsPerMilliseconds[CONVEYOR_TRIGGER] > 0.7f;

     		// Only record the the moment of trigger, the trigger might persist for some time
     		if (!Entry.PedalAudioOn && PedalHit)
     		{
     			FAudioTimeEntry LocalEntry;
     			LocalEntry.Time = TimeCursor;
     			LocalEntry.PedalAudioOn = PedalHit;
     			Entry.PedalAudioOn = PedalHit;
     			TriggerHits.Add(&LocalEntry);
     			CurrentMusicData->AddRow(FName(*FString::FromInt(i)), LocalEntry);
     		} else if(Entry.PedalAudioOn && !PedalHit)
     		{
     			Entry.PedalAudioOn = PedalHit;
     		}
     		
     		if (!Entry.HandleAudioOn && HandleHit)
     		{
     			FAudioTimeEntry LocalEntry;
     			LocalEntry.Time = TimeCursor;
     			LocalEntry.HandleAudioOn = HandleHit;
     			Entry.HandleAudioOn = HandleHit;
     			TriggerHits.Add(&LocalEntry);
     			CurrentMusicData->AddRow(FName(*FString::FromInt(i)), LocalEntry);
     		} else if(Entry.HandleAudioOn && !HandleHit)
     		{
     			Entry.HandleAudioOn = HandleHit;
     		}
     		
     		if (!Entry.ConveyorAudioOn && ConveyorHit)
     		{
     			FAudioTimeEntry LocalEntry;
     			LocalEntry.Time = TimeCursor;
     			LocalEntry.ConveyorAudioOn = ConveyorHit;
     			Entry.ConveyorAudioOn = ConveyorHit;
     			TriggerHits.Add(&LocalEntry);
     			CurrentMusicData->AddRow(FName(*FString::FromInt(i)), LocalEntry);
     		} else if(Entry.ConveyorAudioOn && !ConveyorHit)
     		{
     			Entry.ConveyorAudioOn = ConveyorHit;
     		}


     	}
     	// write datatable out to csv
     	FString TableCSV = CurrentMusicData->GetTableAsCSV();
     	UE_LOG(LogTemp, Warning, TEXT("%s"), *FPaths::ProjectDir());
     	// CSV must be in UTF8 format
     	FFileHelper::SaveStringToFile(TableCSV, *CSVPath, FFileHelper::EEncodingOptions::ForceUTF8);
	}
	// TODO:: let trigger manager subscribe here
	// SpawnedTriggers = SpawnTriggers();
	OnCompleteAudioAnalyze.Broadcast();
}

void AMusicManager::CheckImportedAudio(URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave, ERuntimeImportStatus Status)
{
	if (Status == ERuntimeImportStatus::SuccessfulImport)
	{
		Audio->SetSound(ImportedSoundWave);
		MusicDuration = ImportedSoundWave->Duration;
		// ImportedSoundWave->OnGeneratePCMData.AddDynamic(this, &ATriggerManager::ProcessImportedAudio);
		AudioPreAnalyze();
	}
}

float AMusicManager::GetMusicPlayTime()
{
	return CurrentPlayTime;
}

void AMusicManager::ReactToAudio(const USoundWave* PlayingSoundWave, const float PlaybackPercent)
{
	// UE_LOG(LogTemp, Warning, TEXT("Analyzing Reaction"));
	TArray<float> AnalyzerOut;
	CurrentPlayTime = MusicDuration * PlaybackPercent;

	
	AudioAnalyzer->GetNormalizedChannelConstantQAtTime(CurrentPlayTime, 0, AnalyzerOut);
	// UE_LOG(LogTemp, Warning, TEXT("%d Time: %f Analyze out: %f %f %f"), AnalyzerOut.Num(), CurrentPlayTime, AnalyzerOut[0], AnalyzerOut[1], AnalyzerOut[2]);
	float Pedal = 0.f;
	float Handel = 0.f;
	float Conveyor = 0.f;
	// UE_LOG(LogTemp, Warning, TEXT("Time： %f"), CurrentPlayTime);
	if (AnalyzerOut[PEDAL_TRIGGER] > 0.6)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Pedal hit"));
		Pedal = 1.f;
		
	}
	if (AnalyzerOut[HANDLE_TRIGGER] > 0.6)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Handel hit"));
		Handel = 1.f;

	}
	if (AnalyzerOut[CONVEYOR_TRIGGER] > 0.7)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Conveyor hit"));
		Conveyor = 1.f;

	}
	DebugPedal->SetWorldScale3D(FVector(1.f, 1.f, Pedal * 2.f));
	DebugHandle->SetWorldScale3D(FVector(1.f, 1.f, Handel * 2.f));
	DebugConveyor->SetWorldScale3D(FVector(1.f, 1.f, Conveyor * 2.f));
	
	float time = FMath::TruncToInt(CurrentPlayTime * 100.f) / 100.f;
	
	// UE_LOG(LogTemp, Warning, TEXT("percent %f, time: %f Time orgin: %f aiming time: %f"), PlaybackPercent, time, CurrentPlayTime, TriggerHits[Count]->Time);
	
	// if (FMath::Abs(time - TriggerHits[Count]->Time) < 0.02)
	// {
	// 	
	// 	Objs[0]->SetActorLocation(SpawnedTriggers[count]->GetActorLocation());
	// 	UE_LOG(LogTemp, Warning, TEXT("coutn: %d debug loc: %s"), Count, *SpawnedTriggers[Count]->GetActorLocation().ToString());
	// 	Count++;
	// }

	// for (int i = 0; i < 48; i++)
	// {
	// 	Objs[i]->SetActorScale3D(FVector(1.f, 1.f, AnalyzerOut[i] * 2.f));
	// }
}
