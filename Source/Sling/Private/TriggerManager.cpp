// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerManager.h"

#include <fstream>

#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ATriggerManager::ATriggerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void ATriggerManager::BeginPlay()
{
	Super::BeginPlay();

	CurrentMusicData = NewObject<UDataTable>(this, UDataTable::StaticClass(), TEXT("CurrentMusicData"));
	CurrentMusicData->RowStruct = FAudioTimeEntry::StaticStruct();
	
	// for (int i = 0; i < 48; i++)
	// {
	// 	FVector loc = FVector(-100.f, i*50.f, 100.f);
	// 	AStaticMeshActor *spawned = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
	// 	spawned->SetMobility(EComponentMobility::Movable);
	// 	spawned->SetActorLocation(loc);
	// 	spawned->GetStaticMeshComponent()->SetStaticMesh(AStaticMesh->GetStaticMesh());
	// 	Objs.Add(spawned);
	// }
	
	FillMusicDataTable();
	MusicManager = Cast<AMusicManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMusicManager::StaticClass()));
	MusicManager->OnCompleteAudioAnalyze.AddDynamic(this, &ATriggerManager::SpawnTriggers);
	
}

// Called every frame
void ATriggerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ATriggerManager::FillMusicDataTable()
{
	FString CSVPath = FPaths::ProjectDir()+TEXT("Content/MusicData/")+MusicName.ToString()+TEXT(".csv");
	if(FPlatformFileManager::Get().GetPlatformFile().FileExists(*FPaths::ConvertRelativePathToFull(CSVPath)))
	{
		UDataTableFunctionLibrary::FillDataTableFromCSVFile(CurrentMusicData, CSVPath);
		CurrentMusicData->GetAllRows(TEXT("MusicDataTable"), TriggerHits);
	}
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

			// UE_LOG(LogTemp, Warning, TEXT("%d Pedal rotation %s Angle %f speed %f fRotator %s "), PedalCursor, *SpawnedTriggers[PedalCursor]->GetActorRotation().ToString(), Angle, Speed, *Rot.ToString());
		}
		else
		{
			// FRotator AbsRotation = UKismetMathLibrary::FindLookAtRotation(SpawnedTriggers[PedalCursor]->GetActorLocation(), SpawnedTriggers[PedalCursor + 1]->GetActorLocation());
			// Spawned->SetAngle(AbsRotation);

		}
	}
}


void ATriggerManager::SpawnTriggers()
{

	bool DoubleGap = true;
	float Direction = 1.f;
	
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
		
		SpawnedTriggers[cursor]->TimeStamp = TriggerHits[cursor]->Time;
	}
}
