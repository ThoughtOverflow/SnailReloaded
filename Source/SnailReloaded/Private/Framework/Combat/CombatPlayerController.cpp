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
		PlayerHud->bIsReloading = HudData->GetIsReloading();
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
		SetReloading(PlayerHud->bIsReloading);
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
			MenuWidgetsRef.Add(BuyMenu);
		}
		if(BuyMenu)
		{
			if(bOpen)
			{
				BuyMenu->AddToViewport();
				SetShowMouseCursor(true);
				SetInputMode(FInputModeGameAndUI());
				ActivateUIInputHander(true);
				
			}else
			{
				if(BuyMenu->IsInViewport()) BuyMenu->RemoveFromParent();
				SetShowMouseCursor(false);
				SetInputMode(FInputModeGameOnly());
				ActivateUIInputHander(false);
			}
		}
	}
}



