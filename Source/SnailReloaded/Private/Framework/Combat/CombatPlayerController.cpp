// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/CombatPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Components/ArmoredHealthComponent.h"
#include "Framework/Combat/CombatPlayerState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Gameplay/UI/BuyMenu.h"
#include "Gameplay/UI/HudData.h"
#include "Gameplay/UI/PlayerHud.h"


ACombatPlayerController::ACombatPlayerController()
{
	
}

void ACombatPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//If player character is possessed, callback to initialize
	if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(InPawn))
	{
		if(IsLocalController())
		{
			CreatePlayerHud();
		}else
		{
			Client_CreatePlayerHud();
		}
		PlayerCharacter->OnPlayerPossessed(this);
		
	}
}

void ACombatPlayerController::CloseLastOpenMenu()
{
	//Last menu with cursor enabled
	if(MenuWidgetsRef.Num() == 1)
	{
		SetShowMouseCursor(false);
		SetInputMode(FInputModeGameOnly());
		ActivateUIInputHander(false);
	}
	Super::CloseLastOpenMenu();
}

void ACombatPlayerController::ActivateUIInputHander(bool bActivate)
{
	Super::ActivateUIInputHander(bActivate);
	//Add player character input blocks as well.
	if(bActivate)
	{
		Cast<ADefaultPlayerCharacter>(GetPawn())->BlockPlayerInputs(true);
	}else
	{
		Cast<ADefaultPlayerCharacter>(GetPawn())->BlockPlayerInputs(false);
	}
	
}


void ACombatPlayerController::CreatePlayerHud()
{
	if(IsLocalController())
	{
		if(!PlayerHud && PlayerHudClass) PlayerHud = CreateWidget<UPlayerHud>(this, PlayerHudClass);
		if(PlayerHud) PlayerHud->AddToViewport();
	}
}

void ACombatPlayerController::Client_CreatePlayerHud_Implementation()
{
	CreatePlayerHud();
}

void ACombatPlayerController::UpdatePlayerHud(UHudData* HudData)
{
	if(IsLocalController() && PlayerHud)
	{
		PlayerHud->PlayerHealthPercentage = HudData->GetPlayerHealthPercentage();
		PlayerHud->CurrentClipAmmo = HudData->GetCurrentClipAmmo();
		PlayerHud->CurrentTotalAmmo = HudData->GetCurrentTotalAmmo();
		PlayerHud->CurrentWeaponName = HudData->GetCurrentWeaponName();
		PlayerHud->PlayerShieldHealth = HudData->GetPlayerShieldHealth();
		PlayerHud->bIsReloading = HudData->GetIsReloading();
		PlayerHud->bShowPlantHint = HudData->GetShowPlantHint();
	}
}

UHudData* ACombatPlayerController::GetHudData()
{
	UHudData* HudData = UHudData::CreatePlayerHudWrapper(this);
	if(IsLocalController() && PlayerHud)
	{
		HudData->SetCurrentClipAmmo(PlayerHud->CurrentClipAmmo)->
		SetCurrentTotalAmmo(PlayerHud->CurrentTotalAmmo)->
		SetCurrentWeaponName(PlayerHud->CurrentWeaponName)->
		SetPlayerHealthPercentage(PlayerHud->PlayerHealthPercentage)->
		SetPlayerShieldHealth(PlayerHud->PlayerShieldHealth)->
		SetReloading(PlayerHud->bIsReloading)->
		SetShowPlantHint(PlayerHud->bShowPlantHint);
	}
	return HudData;
}

void ACombatPlayerController::ToggleBuyMenu(bool bOpen)
{
	if(IsLocalController())
	{
		if(!BuyMenu && BuyMenuClass)
		{
			BuyMenu = CreateWidget<UBuyMenu>(this, BuyMenuClass);
		}
		if(BuyMenu)
		{
			if(bOpen)
			{
				BuyMenu->AddToViewport();
				SetShowMouseCursor(true);
				SetInputMode(FInputModeGameAndUI());
				ActivateUIInputHander(true);
				MenuWidgetsRef.Add(BuyMenu);
				
			}else
			{
				if(BuyMenu->IsInViewport())
				{
					BuyMenu->RemoveFromParent();
					SetShowMouseCursor(false);
					SetInputMode(FInputModeGameOnly());
					ActivateUIInputHander(false);
					if(MenuWidgetsRef.Contains(BuyMenu)) MenuWidgetsRef.Remove(BuyMenu);
				}
			}
		}
	}else
	{
		Client_ToggleBuyMenu(bOpen);
	}
}

void ACombatPlayerController::ShowDeathScreen()
{
	if(IsLocalController())
	{
		if(!DeathScreen && DeathScreenClass)
		{
			DeathScreen = CreateWidget<UUserWidget>(this, DeathScreenClass);
		}
		if(DeathScreen && !DeathScreen->IsInViewport())
		{
			DeathScreen->AddToViewport();	
		}
	}else
	{
		Client_ShowDeathScreen();
	}
}

void ACombatPlayerController::Client_ShowDeathScreen_Implementation()
{
	ShowDeathScreen();
}

void ACombatPlayerController::Client_ToggleBuyMenu_Implementation(bool bOpen)
{
	ToggleBuyMenu(bOpen);
}



