// Copyright Epic Games, Inc. All Rights Reserved.


#include "SlingGameModeBase.h"

ASlingGameModeBase::ASlingGameModeBase()
	: Super()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_Character"));
	// DefaultPawnClass = PlayerPawnClassFinder.Class;
	DefaultPawnClass = PawnClass;
}
