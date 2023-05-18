// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/SnailGameInstance.h"

#include "Framework/Savegames/SettingsSavegame.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

USnailGameInstance::USnailGameInstance()
{
	
}

void USnailGameInstance::Init()
{
	Super::Init();

	InitializeSavegames();
	
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
	}
}
