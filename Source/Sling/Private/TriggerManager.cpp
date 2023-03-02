// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerManager.h"

#include <fstream>

#include "Engine/StaticMeshActor.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ATriggerManager::ATriggerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	Audio->SetupAttachment(RootComponent);
	Audio->SetAutoActivate(false);
	
	// static ConstructorHelpers::FObjectFinderOptional<USoundBase> SoundFinder(TEXT("SoundWave'/Game/Audio/MusicLibrary/Forget.Forget'"));
	// if (SoundFinder.Succeeded())
	// {
	// 	// Music = SoundFinder.Get();
	// 	// Audio->SetSound(Music);
	// 	MusicDuration = SoundFinder.Get()->Duration;
	// }else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Cannot Find Music."));
	// }

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

	DebugHandle->SetupAttachment(Audio);
	DebugConveyor->SetupAttachment(Audio);
	DebugPedal->SetupAttachment(Audio);
	DebugHiHat->SetupAttachment(Audio);

	AStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("TestMesh");
	AStaticMesh->SetupAttachment(Audio);

	DebugConveyor->SetRelativeLocation(FVector(0 ,200, 0));
	DebugHandle->SetRelativeLocation(FVector(0 ,0, 0));
	DebugPedal->SetRelativeLocation(FVector(0 ,-200, 0));
	

	
}

// Called when the game starts or when spawned
void ATriggerManager::BeginPlay()
{
	Super::BeginPlay();
	DebugHiHat->SetWorldLocation(FVector(0 ,0, 80));
		
	// Import Audio from file
	// AudioToolAnalyzer = UAudioAnalysisToolsLibrary::CreateAudioAnalysisTools(4096, EAnalysisWindowType::HammingWindow);
	AudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();
	AudioImporter->OnResult.AddDynamic(this, &ATriggerManager::CheckImportedAudio);
	AudioImporter->ImportAudioFromFile(ImportAudioPath, ERuntimeAudioFormat::Auto);


	count = 0;
	Audio->OnAudioPlaybackPercent.AddDynamic(this, &ATriggerManager::ReactToAudio);

	// for (int i = 0; i < 48; i++)
	// {
	// 	FVector loc = FVector(-100.f, i*50.f, 100.f);
	// 	AStaticMeshActor *spawned = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
	// 	spawned->SetMobility(EComponentMobility::Movable);
	// 	spawned->SetActorLocation(loc);
	// 	spawned->GetStaticMeshComponent()->SetStaticMesh(AStaticMesh->GetStaticMesh());
	// 	Objs.Add(spawned);
	// }

	AStaticMeshActor *spawned = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
	spawned->SetMobility(EComponentMobility::Movable);
	spawned->SetActorLocation(FVector(0,0,80));
	spawned->GetStaticMeshComponent()->SetStaticMesh(AStaticMesh->GetStaticMesh());
	Objs.Add(spawned);
}

// Called every frame
void ATriggerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATriggerManager::AudioPreAnalyze()
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
     		bool PedalHit = StrengthsPerMilliseconds[PEDAL_TRIGGER] > 0.6f;
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
	SpawnedTriggers = SpawnTriggers();
}

void ATriggerManager::CheckImportedAudio(URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave, ERuntimeImportStatus Status)
{
	if (Status == ERuntimeImportStatus::SuccessfulImport)
	{
		Audio->SetSound(ImportedSoundWave);
		MusicDuration = ImportedSoundWave->Duration;
		// ImportedSoundWave->OnGeneratePCMData.AddDynamic(this, &ATriggerManager::ProcessImportedAudio);
		AudioPreAnalyze();
	}
}

void ATriggerManager::ProcessImportedAudio(const TArray<float>& PCMData)
{
	AudioToolAnalyzer->ProcessAudioFrame(PCMData, true);
	int Pedal = AudioToolAnalyzer->IsBeatRange(0,2, 1) == true ? 2 : 1;
	int Handle = AudioToolAnalyzer->IsSnare() == true ? 2 : 1;
	int Conveyor = AudioToolAnalyzer->IsKick() == true ? 2 : 1;
	int Hihat = AudioToolAnalyzer->IsHiHat() == true ? 2 : 1;
	
	DebugPedal->SetWorldScale3D(FVector(1.f, 1.f, Pedal));
	DebugHandle->SetWorldScale3D(FVector(1.f, 1.f, Handle));
	DebugConveyor->SetWorldScale3D(FVector(1.f, 1.f, Conveyor));
	DebugHiHat->SetWorldScale3D(FVector(1.f, 1.f, Hihat));
}

void ATriggerManager::SetPedalDistanceAngle(int PedalCursor)
{

	// determine the previous pedal's angle
	ALaunchPedal *Spawned = Cast<ALaunchPedal>(SpawnedTriggers[PedalCursor]);

	if (PedalCursor + 1 < TriggerHits.Num())
	{
		float TimeInAir = TriggerHits[PedalCursor + 1]->Time - TriggerHits[PedalCursor]->Time;
		float Speed = TimeInAir * 800.f;
		Spawned->SetSpeed(Speed);
		if (TriggerHits[PedalCursor + 1]->PedalAudioOn)
		{

            // FRotator AbsRotation = UKismetMathLibrary::FindLookAtRotation(SpawnedTriggers[PedalCursor]->GetActorLocation(), SpawnedTriggers[PedalCursor + 1]->GetActorLocation());
            // Pedals face the direction of launch
            // SpawnedTriggers[PedalCursor]->SetActorRotation(AbsRotation);
            float Angle = Spawned->ComputeAngle(TimeInAir, Speed);
            FRotator Rot = FRotator(Spawned->TransformAngleToWorldSpace(Angle));
            Spawned->SetAngle(Rot);
			Spawned->SetFlyDistance(Spawned->ComputeDistance(Angle, Speed, 0.f));

			UE_LOG(LogTemp, Warning, TEXT("%d Pedal rotation %s Angle %f speed %f fRotator %s "), PedalCursor, *SpawnedTriggers[PedalCursor]->GetActorRotation().ToString(), Angle, Speed, *Rot.ToString());
		}
		else
		{
			// FRotator AbsRotation = UKismetMathLibrary::FindLookAtRotation(SpawnedTriggers[PedalCursor]->GetActorLocation(), SpawnedTriggers[PedalCursor + 1]->GetActorLocation());
			// Spawned->SetAngle(AbsRotation);

		}
	}
}


TArray<ATrigger*> ATriggerManager::SpawnTriggers()
{

	bool DoubleGap = true;
	float Direction = 1.f;
	FString CSVPath = FPaths::ProjectDir()+TEXT("Content/MusicData/debug.csv");
	
	for (int cursor = 0; cursor < TriggerHits.Num(); cursor++)
	{
		if(TriggerHits[cursor]->HandleAudioOn)
		{
			if (DoubleGap)
            {
            	DoubleGap = false;
            }
            else
            {
            	DoubleGap = true;
            	TriggerHits.RemoveAt(cursor);
            	cursor -= 1;
            }
		}
	}
	
	for (int cursor = 0; cursor < TriggerHits.Num(); cursor++)
	{
		FAudioTimeEntry *Entry = TriggerHits[cursor];
		if (Entry->PedalAudioOn)
		{

			if (cursor > 0 && TriggerHits[cursor - 1]->PedalAudioOn)
			{
				ALaunchPedal *Spawned = Cast<ALaunchPedal>(SpawnedTriggers[cursor - 1]);

				int SpawnLoc = DistancesToStart[DistancesToStart.Num()-1] + Spawned->GetFlyDistance();
				DistancesToStart.Add(SpawnLoc);
			}
			else
			{
				if (cursor == 0)
				{
					DistancesToStart.Add(500.f * Entry->Time);
				}
				else
				{
					float Distance = 500.f * Entry->Time - 500.f * TriggerHits[cursor - 1]->Time + DistancesToStart[DistancesToStart.Num()-1];
                    DistancesToStart.Add(Distance);
				}
				
			}
			
		 	SpawnedTriggers.Add(GetWorld()->SpawnActor<ALaunchPedal>(ALaunchPedal::StaticClass(), FVector(0, 0, 0), FRotator(0)));

			SetPedalDistanceAngle(cursor);

			SpawnedTriggers[cursor]->SetActorLocation(FVector(DistancesToStart[cursor], 0, 0));
			
	
		}
		else if (Entry->HandleAudioOn)
		{
			float Distance = 500.f * Entry->Time - 500.f * TriggerHits[cursor - 1]->Time + DistancesToStart[DistancesToStart.Num()-1];
			DistancesToStart.Add(Distance);
			
			SpawnedTriggers.Add(GetWorld()->SpawnActor<ASlingHandle>(ASlingHandle::StaticClass(), FVector(DistancesToStart[DistancesToStart.Num()-1], 200.f * Direction, 300.f), FRotator(0)));
			Direction *= -1.f;

		}
		else if (Entry->ConveyorAudioOn)
		{

			float Distance = 500.f * Entry->Time - 500.f * TriggerHits[cursor - 1]->Time + DistancesToStart[DistancesToStart.Num()-1];
			DistancesToStart.Add(Distance);
			
			SpawnedTriggers.Add(GetWorld()->SpawnActor<ASpeedConveyor>(ASpeedConveyor::StaticClass(), FVector(DistancesToStart[DistancesToStart.Num()-1], 300.f, 0), FRotator(0)));
		}
	}
	return SpawnedTriggers;
}

TArray<float> ATriggerManager::StrengthsToPedalSpawnDistance()
{
	TArray<float> Distances;
	for(FAudioTimeEntry *entry : TriggerHits)
	{
		Distances.Add(entry->Time * 100.f);
	}
	return Distances;
}


void ATriggerManager::ReactToAudio(const USoundWave* PlayingSoundWave, const float PlaybackPercent)
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
	
	UE_LOG(LogTemp, Warning, TEXT("percent %f, time: %f Time orgin: %f aiming time: %f"), PlaybackPercent, time, CurrentPlayTime, TriggerHits[count]->Time);
	
	if (FMath::Abs(time - TriggerHits[count]->Time) < 0.02)
	{
		
		Objs[0]->SetActorLocation(SpawnedTriggers[count]->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("coutn: %d debug loc: %s"), count, *SpawnedTriggers[count]->GetActorLocation().ToString());
		count++;
	}
	


	// for (int i = 0; i < 48; i++)
	// {
	// 	Objs[i]->SetActorScale3D(FVector(1.f, 1.f, AnalyzerOut[i] * 2.f));
	// }
}


