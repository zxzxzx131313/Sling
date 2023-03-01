// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TP_FirstPerson/TP_FirstPersonCharacter.h"
#include "SlingGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SLING_API ASlingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	public:
		ASlingGameModeBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APawn> PawnClass;
};
