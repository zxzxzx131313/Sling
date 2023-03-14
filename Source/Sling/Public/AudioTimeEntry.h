#pragma once


#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AudioTimeEntry.generated.h"

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
