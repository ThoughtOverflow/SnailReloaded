// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SnailGameInstance.generated.h"

#define GOLDEN_SENS (0.130975f);

class USettingsSavegame;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEpicLoginComplete);

class FOnlineSessionSearch;
DECLARE_LOG_CATEGORY_EXTERN(LogOnlineGameSession, Log, All);

UENUM()
enum class ESavegameCategory : uint8
{
	SETTINGS = 0,
	COSMETICS = 1
};

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API USnailGameInstance : public UGameInstance
{

private:
	GENERATED_BODY()

public:

	USnailGameInstance();

	UPROPERTY(BlueprintReadOnly)
	USettingsSavegame* SavedSettings;

	UPROPERTY(BlueprintReadOnly)
	FString MainStatusMessage;

	UPROPERTY(BlueprintReadOnly)
	int32 APIPort;
	UPROPERTY(BlueprintReadOnly)
	FString APIWebAddress;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bUseAlternateAPI;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = bUseAlternateAPI))
	FString AlternateAPIWebAddress;
	
protected:
	

	virtual void Init() override;

	UFUNCTION()
	void InitializeSavegames();

	UFUNCTION()
	void OnAsyncSaveGameFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	UFUNCTION(BlueprintCallable)
	void LoadGameSettings();

	UPROPERTY()
	FUniqueNetIdRepl InstanceNetId;
	
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game Settings")
	bool bRequireLoginInEditor;
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
	
	UPROPERTY()
	bool bIsAuthServer;

protected:

	virtual void Shutdown() override;
	
	/**
	 * @brief Query the command line arguments for dedicated servers.
	 */
	void QueryCommandLineArguments();
	/**
	 * @brief Setup the game as a dedicated server.
	 */
	void SetupDedicatedServer();
	/**
	 * @brief Setup the game as a Standalone game.
	 */
	void SetupGameClient();
	/**
	 * @brief Setup the game as a dev client. Development only!
	 * @param id The id for EOS Dev auth tool.
	 */
	UFUNCTION(Exec)
	void SetupDevClient(int32 id);
	/**
	 * @brief Callback for server search process.
	 * @param bSuccess Whether the search was successful or not.
	 */
	void PostDedicatedSearch(bool bSuccess);
	/**
	 * @brief Get the specified player's configuration savegame.
	 * @param NetId The player's specified net identifier.
	 */
	void GetSavegame(const FUniqueNetId& NetId, ESavegameCategory Category);
	/**
	 * @brief Create a local copy of the specified data array. (For config savegame)
	 * @param downloadedContents The downloaded / loaded data.
	 * @param bHasDownload Whether the savegame got downloaded from the cloud or not.
	 */
	void CreateLocalCopyOfSavegame(TArray<uint8>& downloadedContents, bool bHasDownload, FString FileName, ESavegameCategory Category);
	/**
	 * @brief Save the config savegame.
	 */
	void SaveSavegame(USaveGame* SavegameObj, FString SavegameName);
	/**
	 * @brief Upload the config savegame to the EOS cloud service.
	 */
	void UploadSavegameToCloud(ESavegameCategory Category, FString SavegameName);

	IOnlineSubsystem* OnlineSubsystem;
	
	TSharedPtr<FOnlineSessionSearch> SessionSearchSettings;

	/**
	 * @brief The epic games identifier of the player.
	 */
	UPROPERTY()
	FString PlayerEpicID;

	//delegate binds:

	/**
	 * @brief Callback for epic login completion.
	 */
	void OnLoginComplete(int ControllerIndex, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& ErrorString);
	/**
	 * @brief Callback for dedicated session creation.
	 */
	void DedicatedSessionCreated(FName SessionName, bool bWasSuccessful);
	/**
	 * @brief Callback for session destroyed.
	 */
	void SessionDestroyed(FName SessionName, bool bWasSuccessful);
	/**
	 * @brief Callback for player config download finished.
	 */
	void OnSavegameDownloadFinished(bool bSuccessful, const FUniqueNetId& NetId, const FString& FileName);
	/**
	 * @brief Callback for Player config upload finished.
	 */
	void OnPlayerConfigUploadFinished(bool bSuccessful, const FUniqueNetId& NetId, const FString& FileName);

	
	UPROPERTY()
	bool bSettingsSavegameLoaded;
	UPROPERTY()
	bool bApiAccountDataReady;
	UPROPERTY()
	bool bApiInventoryDataReady;

	UFUNCTION()
	void CheckForTravelReady();

public:

	/**
	 * @brief Search for a server and connect to the first available instance.
	 */
	UFUNCTION(BlueprintCallable)
	void SearchAndConnectToMasterServer();

	/**
	 * @brief Getter for player epic ID
	 * @return The epic games identifier.
	 */
	UFUNCTION(BlueprintGetter)
	FString GetPlayerEpicID();
	UFUNCTION(BlueprintGetter)
	FString GetPlayerAuthToken();

	/**
	 * @brief Callback for session join completion.
	 */
	void OnJoinComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**
	 * @brief Host a session. (Either dedicated or listen)
	 */
	void CreateDedicatedServerSession();

	UFUNCTION(BlueprintCallable)
	void SetSettingsSavegameLoaded(bool bLoaded);
	UFUNCTION(BlueprintCallable)
	void SetApiAccountDataReady(bool bReady);
	UFUNCTION(BlueprintCallable)
	void SetApiInventoryDataReady(bool bReady);

	UFUNCTION(BlueprintCallable)
	void HostServer();
	UFUNCTION(BlueprintCallable)
	void JoinFirstServer();
	
};
