// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SnailGameInstance.generated.h"

class USettingsSavegame;
/**
 * 
 */
UCLASS()
class SNAILRELOADED_API USnailGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	USnailGameInstance();

	UPROPERTY(BlueprintReadOnly)
	USettingsSavegame* SavedSettings;
	
protected:

	virtual void Init() override;

	UFUNCTION()
	void InitializeSavegames();

	UFUNCTION()
	void OnAsyncSaveGameFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	UFUNCTION()
	void LoadGameSettings();
	
public:

	UFUNCTION(BlueprintCallable)
	void SaveSettingsSavegameToDisk();
	
};
