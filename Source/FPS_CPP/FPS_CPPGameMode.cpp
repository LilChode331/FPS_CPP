// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_CPPGameMode.h"
#include "FPS_CPPCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPS_CPPGameMode::AFPS_CPPGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	//DefaultPawnClass = PlayerPawnClassFinder.Class;

}
