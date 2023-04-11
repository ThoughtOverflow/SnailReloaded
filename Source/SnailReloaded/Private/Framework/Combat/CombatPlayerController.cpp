// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/CombatPlayerController.h"

#include "Gameplay/Player/DefaultPlayerCharacter.h"
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
		PlayerCharacter->OnPlayerPossessed(this);
		if(IsLocalController())
		{
			CreatePlayerHud();
		}else
		{
			Client_CreatePlayerHud();
		}
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

void ACombatPlayerController::UpdatePlayerHud(float PlayerHealthPercentage, const FText& WeaponName, int32 CurrentClip, int32 CurrentTotal)
{
	//Temporary;
	if(IsLocalController() && PlayerHud)
	{
		PlayerHud->PlayerHealthPercentage = PlayerHealthPercentage;
		PlayerHud->CurrentClipAmmo = CurrentClip;
		PlayerHud->CurrentTotalAmmo = CurrentTotal;
		PlayerHud->CurrentWeaponName = WeaponName;
	}
}
