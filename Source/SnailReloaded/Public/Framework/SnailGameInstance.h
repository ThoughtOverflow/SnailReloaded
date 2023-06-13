// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SnailGameInstance.generated.h"

#define GOLDEN_SENS (0.130975f);

class USettingsSavegame;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEpicLoginComplete);

class FOnlineSessionSearch;
DECLARE_LOG_CATEGORY_EXTERN(LogOnlineGameSession, Log, All);

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

	UFUNCTION(BlueprintCallable)
	void LoadGameSettings();
	
public:

	UFUNCTION(BlueprintCallable)
	void SaveSettingsSavegameToDisk();

	//EOS related - yoinked from CouncilGame

	/**
 * @addtogroup ds_settings
 * @brief The name of the game server
 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Session Settings")
	FString GameServerName;
	/**
	 * @addtogroup ds_settings
	 * @brief The admin password for the game specified.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Session Settings")
	FString AdminGamePassword;
	/**
	 * @addtogroup ds_settings
	 * @brief Max player count
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Session Settings")
	int32 maxPlayerCount;
	/**
	 * @addtogroup ds_settings
	 * @brief Whether to use developer authentication or not.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game Settings")
	bool bUseDevAuth;
	/**
	 * @addtogroup ds_settings
	 * @brief Reference to the savegame storing the user's config.
	 */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Savegames")
	// UPlayerConfigSavegame* PlayerConfigSavegame;


	//delegates:

	/**
	 * @brief Fired when the login through epic games is finished.
	 */
	UPROPERTY()
	FOnEpicLoginComplete EpicLoginComplete;
	
};
