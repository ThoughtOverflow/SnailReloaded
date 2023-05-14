// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/UI/HudData.h"

#include "Framework/Combat/CombatPlayerController.h"



UHudData::UHudData()
{
	CurrentClipAmmo = 0;
	CurrentTotalAmmo = 0;
	CurrentWeaponName = FText::FromString("");
	PlayerHealthPercentage = -1.f;
	bIsReloading = false;
	WidgetOwner = nullptr;
	
}

UHudData* UHudData::CreatePlayerHudWrapper(ACombatPlayerController* PlayerController)
{
	if(PlayerController)
	{
		UHudData* HudData = NewObject<UHudData>(PlayerController);
		HudData->WidgetOwner = PlayerController;
		return HudData;
	}
	return nullptr;
	
}

UHudData* UHudData::SetPlayerHealthPercentage(float percent)
{
	PlayerHealthPercentage = percent;
	return this;
}

UHudData* UHudData::SetCurrentWeaponName(const FText& NewName)
{
	CurrentWeaponName = NewName;
	return this;
}

UHudData* UHudData::SetCurrentClipAmmo(int32 NewAmmo)
{
	CurrentClipAmmo = NewAmmo;
	return this;
}

UHudData* UHudData::SetCurrentTotalAmmo(int32 NewAmmo)
{
	CurrentTotalAmmo = NewAmmo;
	return this;
}

UHudData* UHudData::SetPlayerShieldHealth(float NewHealth)
{
	PlayerShieldHealth = NewHealth;
	return this;
}

UHudData* UHudData::SetReloading(bool bReloading)
{
	bIsReloading = bReloading;
	return this;
}

UHudData* UHudData::SetShowPlantHint(bool bShow)
{
	bShowPlantHint = bShow;
	return this;
}

bool UHudData::GetShowPlantHint()
{
	return bShowPlantHint;
}

void UHudData::Submit()
{
	if(WidgetOwner && WidgetOwner->IsLocalController())
	{
		WidgetOwner->UpdatePlayerHud(this);
	}
}

float UHudData::GetPlayerHealthPercentage() const
{
	return PlayerHealthPercentage;
}

const FText& UHudData::GetCurrentWeaponName() const
{
	return CurrentWeaponName;
}

int32 UHudData::GetCurrentClipAmmo() const
{
	return CurrentClipAmmo;
}

int32 UHudData::GetCurrentTotalAmmo() const
{
	return CurrentTotalAmmo;
}

float UHudData::GetPlayerShieldHealth() const
{
	return PlayerShieldHealth;
}

bool UHudData::GetIsReloading() const
{
	return bIsReloading;
}
