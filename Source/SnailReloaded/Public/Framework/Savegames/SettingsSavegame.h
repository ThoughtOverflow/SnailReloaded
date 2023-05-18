// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SettingsSavegame.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API USettingsSavegame : public USaveGame
{
	GENERATED_BODY()

public:

	USettingsSavegame();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MouseSensitivity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 FpsCap;
	
};
