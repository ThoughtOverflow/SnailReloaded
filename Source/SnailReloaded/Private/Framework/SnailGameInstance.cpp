// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/SnailGameInstance.h"

#include "Framework/Savegames/SettingsSavegame.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "Framework/DefaultGameMode.h"
#include "Framework/DefaultPlayerController.h"
#include "Framework/DefaultPlayerState.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineUserCloudInterface.h"
#include "Kismet/KismetInternationalizationLibrary.h"


DEFINE_LOG_CATEGORY(LogOnlineGameSession);

void USnailGameInstance::Shutdown()
{
	Super::Shutdown();
	
	if(OnlineSubsystem)
	{
		OnlineSubsystem->GetSessionInterface()->EndSession(FName(*GameServerName));
		OnlineSubsystem->GetSessionInterface()->OnDestroySessionCompleteDelegates.AddUObject(this, &USnailGameInstance::SessionDestroyed);
		OnlineSubsystem->GetSessionInterface()->DestroySession(FName(*GameServerName));
		OnlineSubsystem->GetIdentityInterface()->Logout(0);
	}
}

void USnailGameInstance::QueryCommandLineArguments()
{
	UE_LOG(LogOnlineGameSession, Log, TEXT("Configuring server settings..."))
	FString ServerName;
	FString AdminPassword;
	int32 MaxPlayers;
	if(FParse::Value(FCommandLine::Get(), TEXT("ServerName="), ServerName))
	{
		this->GameServerName = ServerName;
	}
	if(FParse::Value(FCommandLine::Get(), TEXT("AdminPassword="), AdminPassword))
	{
		this->AdminGamePassword = AdminPassword;
	}
	if(FParse::Value(FCommandLine::Get(), TEXT("MaxPlayers="), MaxPlayers))
	{
		this->maxPlayerCount = MaxPlayers;
	}
	UE_LOG(LogOnlineGameSession, Warning, TEXT("Set server name to %s"), *GameServerName);
	UE_LOG(LogOnlineGameSession, Warning, TEXT("Set admin password to %s"), *AdminGamePassword);
	UE_LOG(LogOnlineGameSession, Warning, TEXT("Set max player count to %d"), maxPlayerCount);
}

void USnailGameInstance::SetupDedicatedServer()
{
	CreateDedicatedServerSession();
}

void USnailGameInstance::SetupGameClient()
{
	//steps:
	//log the player in with epic credentials.
	//pull config SaveGame from cloud
	//create a local instance of the savegame to slot1
	//load data from savegame slot 1 for usage.
	//request the API for client information
	if(OnlineSubsystem)
	{
		if(IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			MainStatusMessage = TEXT("Requesting Epic login");
			FOnlineAccountCredentials AccountCredentials;
			AccountCredentials.Id = FString("");
			AccountCredentials.Token = FString("");
			AccountCredentials.Type = FString("accountportal");
			Identity->OnLoginCompleteDelegates->AddUObject(this, &USnailGameInstance::OnLoginComplete);
			Identity->Login(0, AccountCredentials);
		}
	}
}

void USnailGameInstance::SetupDevClient(int32 id)
{
	if(OnlineSubsystem && bUseDevAuth)
	{
		if(IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			FOnlineAccountCredentials AccountCredentials;
			AccountCredentials.Id = FString("localhost:4455");
			AccountCredentials.Token = FString::Printf(TEXT("dev%d"), id);
			AccountCredentials.Type = FString("developer");
			Identity->OnLoginCompleteDelegates->AddUObject(this, &USnailGameInstance::OnLoginComplete);
			Identity->Login(0, AccountCredentials);
		}
	}
}

void USnailGameInstance::PostDedicatedSearch(bool bSuccess)
{
	if(bSuccess)
	{
		UE_LOG(LogOnlineGameSession, Warning, TEXT("Num of sessions: %d"), SessionSearchSettings->SearchResults.Num());
		if(SessionSearchSettings->SearchResults.Num() > 0)
		{
			FOnlineSessionSearchResult& SearchResult = SessionSearchSettings->SearchResults[0];
			if(SearchResult.Session.SessionSettings.bIsDedicated||true)
			{
				if(OnlineSubsystem)
				{
					OnlineSubsystem->GetSessionInterface()->OnJoinSessionCompleteDelegates.AddUObject(this, &USnailGameInstance::OnJoinComplete);
					OnlineSubsystem->GetSessionInterface()->JoinSession(*InstanceNetId.GetUniqueNetId(), FName(*GameServerName), SearchResult);
				}	
			}
		}
		
	}else
	{
		UE_LOG(LogOnlineGameSession, Error, TEXT("ERROR"));
	}
	if(OnlineSubsystem)
	{
		if(IOnlineSessionPtr SessionManager = OnlineSubsystem->GetSessionInterface())
		{
			SessionManager->ClearOnFindSessionsCompleteDelegates(this);
		}
	}
}

void USnailGameInstance::GetSavegame(const FUniqueNetId& NetId, ESavegameCategory Category)
{
	if(OnlineSubsystem)
	{
		if(IOnlineUserCloudPtr UserCloud = OnlineSubsystem->GetUserCloudInterface())
		{
			MainStatusMessage = TEXT("Downloading settings...");
			UserCloud->OnReadUserFileCompleteDelegates.AddUObject(this, &USnailGameInstance::OnSavegameDownloadFinished);
			switch (Category) {
			case ESavegameCategory::SETTINGS: UserCloud->ReadUserFile(NetId, TEXT("SettingsSavegame.sav")); break;
			case ESavegameCategory::COSMETICS: break;
			default: ;
			}
		}
	}
}

void USnailGameInstance::CreateLocalCopyOfSavegame(TArray<uint8>& downloadedContents, bool bHasDownload, FString FileName, ESavegameCategory Category)
{
	FString SlotName = FileName.LeftChop(4);
	if(bHasDownload)
	{
		MainStatusMessage = TEXT("Creating local save");
		if(USaveGame* MemoLoad = Cast<USaveGame>(UGameplayStatics::LoadGameFromMemory(downloadedContents)))
		{
			switch (Category) {
			case ESavegameCategory::SETTINGS: SavedSettings = Cast<USettingsSavegame>(MemoLoad); SaveSavegame(SavedSettings, SlotName); break;
			case ESavegameCategory::COSMETICS: break;
			default: ;
			}
			//then save to a local instance
			
		}else
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldnt load from memo!"));
		}
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("No downloaded instance. Checking for local savegame existance."))
		if(UGameplayStatics::DoesSaveGameExist(SlotName, 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("Loading locally found savegame."));
			if(USaveGame* LoadedSave = Cast<USaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0)))
			{
				switch (Category) {
				case ESavegameCategory::SETTINGS: SavedSettings = Cast<USettingsSavegame>(LoadedSave); break;
				case ESavegameCategory::COSMETICS: break;
				default: ;
				}
				UploadSavegameToCloud(Category, FileName);
			}
		}else
		{
			UE_LOG(LogTemp, Warning, TEXT("Creating default savegame."));
			UClass* SavegameClass = nullptr;
			switch (Category) {
			case ESavegameCategory::SETTINGS: SavegameClass = USettingsSavegame::StaticClass(); break;
			case ESavegameCategory::COSMETICS: break;
			default: ;
			}
			if(USaveGame* NewPlayerConfig = Cast<USaveGame>(UGameplayStatics::CreateSaveGameObject(SavegameClass)))
			{
				//save to disk:
				switch (Category) {
				case ESavegameCategory::SETTINGS: SavedSettings = Cast<USettingsSavegame>(NewPlayerConfig); break;
				case ESavegameCategory::COSMETICS: break;
				default: ;
				}
				SaveSavegame(NewPlayerConfig, SlotName);
				UploadSavegameToCloud(Category, FileName);
			}
		}
	}
	SetSettingsSavegameLoaded(true);
}

void USnailGameInstance::SaveSavegame(USaveGame* SavegameObj, FString SavegameName)
{
	//async for faster time.
	FAsyncSaveGameToSlotDelegate AsyncSaveDelegate;
	AsyncSaveDelegate.BindUObject(this, &USnailGameInstance::OnAsyncSaveGameFinished);
	MainStatusMessage = TEXT("Saving...");
	UGameplayStatics::AsyncSaveGameToSlot(SavegameObj, SavegameName, 0, AsyncSaveDelegate);
}

void USnailGameInstance::UploadSavegameToCloud(ESavegameCategory Category, FString SavegameName)
{
	UE_LOG(LogTemp, Warning, TEXT("Uploading new save file..."))
	if(OnlineSubsystem)
	{
		if(IOnlineUserCloudPtr UserCloud = OnlineSubsystem->GetUserCloudInterface())
		{
			USaveGame* SaveGame = nullptr;
			switch (Category) {
			case ESavegameCategory::SETTINGS: SaveGame = SavedSettings; break;
			case ESavegameCategory::COSMETICS: break;
			default: ;
			}
			TArray<uint8> saveContents;
			if(UGameplayStatics::SaveGameToMemory(SaveGame, saveContents))
			{
				UE_LOG(LogTemp, Warning, TEXT("File Size: %d"), saveContents.Num());
				UserCloud->OnWriteUserFileCompleteDelegates.AddUObject(this, &USnailGameInstance::OnPlayerConfigUploadFinished);
				UserCloud->WriteUserFile(*InstanceNetId.GetUniqueNetId(), SavegameName, saveContents);	
			}
		}
	}
}

void USnailGameInstance::OnLoginComplete(int ControllerIndex, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& ErrorString)
{
	if(bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("HOLa"));
		UserId.ToString().Split(TEXT("|"), &PlayerEpicID, nullptr);
		// PlayerNetID = UserId;
		UE_LOG(LogOnlineGameSession, Warning, TEXT("Found eid: %s"), *PlayerEpicID);
		UE_LOG(LogOnlineGameSession, Warning, TEXT("Found full ID: %s"), *UserId.ToString());
		MainStatusMessage = TEXT("Login complete");
		//Get the settings.
		InstanceNetId = UserId;
		GetSavegame(UserId, ESavegameCategory::SETTINGS);
		EpicLoginComplete.Broadcast();
				
	}else
	{
		UE_LOG(LogOnlineGameSession, Error, TEXT("Login failed!"));
	}
	OnlineSubsystem->GetIdentityInterface()->ClearOnLoginCompleteDelegates(0, this);
}

void USnailGameInstance::DedicatedSessionCreated(FName SessionName, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		UE_LOG(LogOnlineGameSession, Log, TEXT("Created dedicated session (%s)"), *SessionName.ToString());
		UE_LOG(LogOnlineGameSession, Warning, TEXT("Registered as authenticated server: %d"), bIsAuthServer);
		GetWorld()->ServerTravel(TEXT("Shipyard_v1?listen"));
	}else
	{
		UE_LOG(LogOnlineGameSession, Error, TEXT("Failed to create dedicated session!"));
	}
	OnlineSubsystem->GetSessionInterface()->ClearOnCreateSessionCompleteDelegates(this);
}

void USnailGameInstance::SessionDestroyed(FName SessionName, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		UE_LOG(LogOnlineGameSession, Log, TEXT("Destroyed session."));
	}
	OnlineSubsystem->GetSessionInterface()->ClearOnDestroySessionCompleteDelegates(this);
}

void USnailGameInstance::OnSavegameDownloadFinished(bool bSuccessful, const FUniqueNetId& NetId,
	const FString& FileName)
{
	if(OnlineSubsystem)
	{
		if(IOnlineUserCloudPtr Cloud = OnlineSubsystem->GetUserCloudInterface())
		{
			Cloud->ClearOnReadUserFileCompleteDelegates(this);
			//TODO: Read file contents from save
			TArray<uint8> fileContents;
			if(bSuccessful) //if false, file couldn't be downloaded, create a new instance or keep the currently existing one.
				{
				MainStatusMessage = TEXT("Download complete.");
				//Read cloudSaveData, and parse into array.
				Cloud->GetFileContents(NetId, FileName, fileContents);
				}
			if(FileName == TEXT("SettingsSavegame.sav")) {
				CreateLocalCopyOfSavegame(fileContents, bSuccessful, FileName, ESavegameCategory::SETTINGS);
			}
		}
	}
}

void USnailGameInstance::OnPlayerConfigUploadFinished(bool bSuccessful, const FUniqueNetId& NetId,
	const FString& FileName)
{
	if(bSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("File upload finished"));
	}
	if(IOnlineUserCloudPtr Cloud = OnlineSubsystem->GetUserCloudInterface())
	{
		Cloud->ClearOnWriteUserFileCompleteDelegates(this);
	}
}

void USnailGameInstance::CheckForTravelReady()
{
	if(bSettingsSavegameLoaded && bApiAccountDataReady && bApiInventoryDataReady)
	{
		if(ADefaultPlayerController* DefaultPlayerController = Cast<ADefaultPlayerController>(GetFirstLocalPlayerController(GetWorld())))
		{
			DefaultPlayerController->ToggleGameInitWidget(false);
		}
		GetWorld()->ServerTravel(TEXT("MainMenu"));
	}
}

void USnailGameInstance::SearchAndConnectToMasterServer()
{
	if(OnlineSubsystem)
	{
		if(IOnlineSessionPtr SessionManager = OnlineSubsystem->GetSessionInterface())
		{
			SessionSearchSettings = MakeShareable(new FOnlineSessionSearch());

			SessionSearchSettings->bIsLanQuery = false;

			SessionManager->OnFindSessionsCompleteDelegates.AddUObject(this, &USnailGameInstance::PostDedicatedSearch);
			SessionManager->FindSessions(0, SessionSearchSettings.ToSharedRef());
		}
	}
}

FString USnailGameInstance::GetPlayerEpicID()
{
	if(!GetWorld()->IsPlayInEditor() || bRequireLoginInEditor)
	{
		return PlayerEpicID;
	}else 
	{
#if WITH_EDITOR
		return "05d5693c39d64934988ef54990074d56";
#else
		return "";
#endif
	}
}

FString USnailGameInstance::GetPlayerAuthToken()
{
	if(OnlineSubsystem)
	{
		return OnlineSubsystem->GetIdentityInterface()->GetAuthToken(0);
	}
	return "";
}

void USnailGameInstance::OnJoinComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(Result == EOnJoinSessionCompleteResult::Success)
	{
		if(OnlineSubsystem)
		{
			if(IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				FString ConnectionString = FString();
				SessionPtr->GetResolvedConnectString(SessionName, ConnectionString);
				if(!ConnectionString.IsEmpty())
				{
					if(APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
					{
						if(ADefaultPlayerState* DefaultPlayerState = PlayerController->GetPlayerState<ADefaultPlayerState>())
						{
							DefaultPlayerState->API_GetPlayerInventoryData();
						}
						PlayerController->SetInputMode(FInputModeGameOnly());
						PlayerController->SetShowMouseCursor(false);
						PlayerController->ClientTravel(ConnectionString, ETravelType::TRAVEL_Absolute);
					}
				}
			}
		}
	}
}

void USnailGameInstance::CreateDedicatedServerSession()
{
	if(OnlineSubsystem)
	{
		if(IOnlineSessionPtr SessionManager = OnlineSubsystem->GetSessionInterface())
		{
			FOnlineSessionSettings CreatedSessionSettings;
			CreatedSessionSettings.bIsDedicated = IsDedicatedServerInstance();
			CreatedSessionSettings.bIsLANMatch = false;
			CreatedSessionSettings.bShouldAdvertise = true;
			CreatedSessionSettings.NumPublicConnections = maxPlayerCount;
			CreatedSessionSettings.bAllowJoinInProgress = false;
			CreatedSessionSettings.bAllowJoinViaPresence = !IsDedicatedServerInstance();
			CreatedSessionSettings.bUsesPresence = !IsDedicatedServerInstance();
			CreatedSessionSettings.Set(SEARCH_KEYWORDS, GameServerName, EOnlineDataAdvertisementType::ViaOnlineService);
			CreatedSessionSettings.Set(FName(TEXT("BISAUTHSERVER")), bIsAuthServer, EOnlineDataAdvertisementType::ViaOnlineService);
			SessionManager->OnCreateSessionCompleteDelegates.AddUObject(this, &USnailGameInstance::DedicatedSessionCreated);
			
			SessionManager->CreateSession(0, FName(*GameServerName), CreatedSessionSettings);
		}
	}
}


USnailGameInstance::USnailGameInstance()
{
	GameServerName = "Council Game Server - Default settings";
	AdminGamePassword = "admincode123";
	maxPlayerCount = 10;
	bIsAuthServer = false;
	bUseDevAuth = false;
	PlayerEpicID = "";
	MainStatusMessage = TEXT("Loading...");
	APIWebAddress = "http://oregtolgy-panzio.com";
	APIPort = 3000;
}

void USnailGameInstance::Init()
{
	Super::Init();

	// InitializeSavegames();

	OnlineSubsystem = IOnlineSubsystem::Get("EOS");
	
	if(OnlineSubsystem)
	{
		UE_LOG(LogOnlineGameSession, Log, TEXT("Created Online Subsystem handler."));
		if(IsDedicatedServerInstance())
		{
			//Query the default command line arguments, if the game is running as a dedicated server.
			QueryCommandLineArguments();
			SetupDedicatedServer();
		}else
		{
			if(!GetWorld()->IsPlayInEditor())
			{
				if(!bUseDevAuth)
				{
					SetupGameClient();
				}
			}else if(bRequireLoginInEditor)
			{
				SetupGameClient();
			}else
			{
				EpicLoginComplete.Broadcast();
			}
		}
	}else
	{
		UE_LOG(LogOnlineGameSession, Error, TEXT("Could not create Online Subsystem handler!"));
	}
	
}

void USnailGameInstance::InitializeSavegames()
{
	if(UGameplayStatics::DoesSaveGameExist(TEXT("GameSettings"), 0))
	{
		//read the config, load variables;
		if(USettingsSavegame* LoadedSave = Cast<USettingsSavegame>(UGameplayStatics::LoadGameFromSlot(TEXT("GameSettings"), 0)))
		{
			SavedSettings = LoadedSave;
		}
	}else
	{
		//Create default savegame:
		if(USettingsSavegame* SettingsSavegame = Cast<USettingsSavegame>(UGameplayStatics::CreateSaveGameObject(USettingsSavegame::StaticClass())))
		{
			SavedSettings = SettingsSavegame;

			//Save it:
			SaveSettingsSavegameToDisk();
			
		}
	}
	if(SavedSettings)
	{
		//If we have the savegame, apply the properties:
		LoadGameSettings();
	}
}

void USnailGameInstance::OnAsyncSaveGameFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	if(bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Saved savegame '%s'"), *SlotName);
		MainStatusMessage = TEXT("Save complete.");
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not save savegame '%s'!"), *SlotName);
	}
	
}

void USnailGameInstance::SaveSettingsSavegameToDisk()
{
	if(SavedSettings)
	{
		FAsyncSaveGameToSlotDelegate AsyncSaveDelegate;
		UploadSavegameToCloud(ESavegameCategory::SETTINGS, TEXT("SettingsSavegame.sav"));
		AsyncSaveDelegate.BindUObject(this, &USnailGameInstance::OnAsyncSaveGameFinished);
		UGameplayStatics::AsyncSaveGameToSlot(SavedSettings, TEXT("SettingsSavegame"), 0, AsyncSaveDelegate);
	}
}

void USnailGameInstance::LoadGameSettings()
{
	if(UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameUserSettings->SetFrameRateLimit(SavedSettings->FpsCap);
		GameUserSettings->ApplySettings(false);
		UKismetInternationalizationLibrary::SetCurrentLanguageAndLocale(SavedSettings->ActiveLanguage, true);
	}
}

void USnailGameInstance::SetSettingsSavegameLoaded(bool bLoaded)
{
	if(bSettingsSavegameLoaded != bLoaded)
	{
		this->bSettingsSavegameLoaded = bLoaded;
		CheckForTravelReady();
	}
}

void USnailGameInstance::SetApiAccountDataReady(bool bReady)
{
	if(bApiAccountDataReady != bReady)
	{
		this->bApiAccountDataReady = bReady;
		CheckForTravelReady();
	}
}

void USnailGameInstance::SetApiInventoryDataReady(bool bReady)
{
	if(bApiInventoryDataReady != bReady)
	{
		this->bApiInventoryDataReady = bReady;
		CheckForTravelReady();
	}
}

void USnailGameInstance::HostServer()
{
	CreateDedicatedServerSession();
}

void USnailGameInstance::JoinFirstServer()
{
	SearchAndConnectToMasterServer();
}
