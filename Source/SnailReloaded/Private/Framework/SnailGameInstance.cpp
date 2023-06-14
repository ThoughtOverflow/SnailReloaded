// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/SnailGameInstance.h"

#include "Framework/Savegames/SettingsSavegame.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineUserCloudInterface.h"
#include "Kismet/KismetInternationalizationLibrary.h"


void USnailGameInstance::Shutdown()
{
	Super::Shutdown();
	
	if(OnlineSubsystem)
	{
		OnlineSubsystem->GetSessionInterface()->EndSession(FName(*GameServerName));
		OnlineSubsystem->GetSessionInterface()->OnDestroySessionCompleteDelegates.AddUObject(this, &USnailGameInstance::SessionDestroyed);
		OnlineSubsystem->GetSessionInterface()->DestroySession(FName(*GameServerName));
	}
}

void USnailGameInstance::QueryCommandLineArguments()
{
	UE_LOG(LogOnlineGameSession, Log, TEXT("Configuring server settings..."))
	FString ServerName;
	FString AdminPassword;
	int32 MaxPlayers;
	int32 AuthServerInt;
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
	if(FParse::Value(FCommandLine::Get(), TEXT("IsAuthServer="), AuthServerInt))
	{
		this->bIsAuthServer = AuthServerInt == 1;
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
					OnlineSubsystem->GetSessionInterface()->JoinSession(*GetFirstGamePlayer()->GetPreferredUniqueNetId(), FName(*GameServerName), SearchResult);
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

void USnailGameInstance::GetPlayerConfigSaveGame(const FUniqueNetId& NetId)
{
	if(OnlineSubsystem)
	{
		if(IOnlineUserCloudPtr UserCloud = OnlineSubsystem->GetUserCloudInterface())
		{
			UserCloud->OnReadUserFileCompleteDelegates.AddUObject(this, &USnailGameInstance::OnPlayerConfigDownloadFinished);
			UserCloud->ReadUserFile(NetId, TEXT("ConfigSavegame.sav"));
		}
	}
}

void USnailGameInstance::CreateLocalCopyOfConfig(TArray<uint8>& downloadedContents, bool bHasDownload)
{
	if(bHasDownload)
	{
		
		if(UPlayerConfigSavegame* MemoLoad = Cast<UPlayerConfigSavegame>(UGameplayStatics::LoadGameFromMemory(downloadedContents)))
		{
			PlayerConfigSavegame = MemoLoad;
			//then save to a local instance
			SavePlayerConfigSavegame();
		}else
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldnt load from memo!"));
		}
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("No downloaded instance. Checking for local savegame existance."))
		if(UGameplayStatics::DoesSaveGameExist(TEXT("PlayerConfig"), 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("Loading locally found savegame."));
			if(UPlayerConfigSavegame* LoadedSave = Cast<UPlayerConfigSavegame>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerConfig"), 0)))
			{
				PlayerConfigSavegame = LoadedSave;
				UploadPlayerConfigToCloud();
			}
		}else
		{
			UE_LOG(LogTemp, Warning, TEXT("Creating default savegame."));
			if(UPlayerConfigSavegame* NewPlayerConfig = Cast<UPlayerConfigSavegame>(UGameplayStatics::CreateSaveGameObject(UPlayerConfigSavegame::StaticClass())))
			{
				//save to disk:
				PlayerConfigSavegame = NewPlayerConfig;
				SavePlayerConfigSavegame();
			}
		}
	}
}

void USnailGameInstance::SavePlayerConfigSavegame()
{
	//async for faster time.
	FAsyncSaveGameToSlotDelegate AsyncSaveDelegate;
	AsyncSaveDelegate.BindUObject(this, &USnailGameInstance::OnAsyncSaveGameFinished);

	UGameplayStatics::AsyncSaveGameToSlot(PlayerConfigSavegame, TEXT("PlayerConfig"), 0, AsyncSaveDelegate);
}

void USnailGameInstance::UploadPlayerConfigToCloud()
{
	UE_LOG(LogTemp, Warning, TEXT("Uploading new save file..."))
	if(OnlineSubsystem)
	{
		if(IOnlineUserCloudPtr UserCloud = OnlineSubsystem->GetUserCloudInterface())
		{
			TArray<uint8> saveContents;
			if(UGameplayStatics::SaveGameToMemory(PlayerConfigSavegame, saveContents))
			{
				UE_LOG(LogTemp, Warning, TEXT("asdasd: %d"), saveContents.Num());
				UserCloud->OnWriteUserFileCompleteDelegates.AddUObject(this, &USnailGameInstance::OnPlayerConfigUploadFinished);
				UserCloud->WriteUserFile(*GetFirstGamePlayer()->GetPreferredUniqueNetId(), TEXT("ConfigSavegame.sav"), saveContents);
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
		GetPlayerConfigSaveGame(UserId);
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

void USnailGameInstance::OnPlayerConfigDownloadFinished(bool bSuccessful, const FUniqueNetId& NetId,
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
				//Read cloudSaveData, and parse into array.
				Cloud->GetFileContents(NetId, TEXT("ConfigSavegame.sav"), fileContents);
				}
			CreateLocalCopyOfConfig(fileContents, bSuccessful);
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
	if(!GetWorld()->IsPlayInEditor() || bUseDevAuth)
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
}

void USnailGameInstance::Init()
{
	Super::Init();

	InitializeSavegames();

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
		AsyncSaveDelegate.BindUObject(this, &USnailGameInstance::OnAsyncSaveGameFinished);
		UGameplayStatics::AsyncSaveGameToSlot(SavedSettings, TEXT("GameSettings"), 0, AsyncSaveDelegate);
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
