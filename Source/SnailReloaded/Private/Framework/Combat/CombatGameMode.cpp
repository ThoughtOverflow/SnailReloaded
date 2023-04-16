// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/CombatGameMode.h"

#include "Components/ArmoredHealthComponent.h"
#include "Components/HealthComponent.h"
#include "Framework/Combat/CombatPlayerState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"

ACombatGameMode::ACombatGameMode()
{
	
}

FDamageResponse ACombatGameMode::ChangeObjectHealth(FDamageRequest DamageRequest)
{
	if(DamageRequest.SourceActor)
	{
		if(UHealthComponent* TargetHealthComponent = Cast<UHealthComponent>(DamageRequest.TargetActor->GetComponentByClass(UHealthComponent::StaticClass())))
		{
			const FDamageResponse Response = TargetHealthComponent->ChangeObjectHealth(DamageRequest);
			return Response;
		}
	}
	return FDamageResponse();
}

bool ACombatGameMode::PurchaseItem(ADefaultPlayerCharacter* PlayerCharacter, EItemIdentifier ItemIdentifier)
{
	if(ItemPriceList.Contains(ItemIdentifier))
	{
		int32 ItemPrice = *ItemPriceList.Find(ItemIdentifier);
		if(ACombatPlayerState* CombatPlayerState = Cast<ACombatPlayerState>(PlayerCharacter->GetPlayerState()))
		{
			if(CombatPlayerState->GetPlayerMoney() >= ItemPrice)
			{
				//Remove monkey:
				CombatPlayerState->ChangePlayerMoney(-ItemPrice);
				if(ItemIdentifier == EItemIdentifier::LightShield || ItemIdentifier == EItemIdentifier::HeavyShield)
				{

					//Shield Purchase:
					if(FShieldProperties* ShieldData = ShieldDataTable.Find(ItemIdentifier))
					{
						PlayerCharacter->UpdateShieldProperties(*ShieldData);
					}
					
					
				}else
				{
					//Weapon purchase:
					if(TSubclassOf<AWeaponBase> WeaponClass = *WeaponIdTable.Find(ItemIdentifier))
					{
						if(!PlayerCharacter->GetWeaponAtSlot(WeaponClass.GetDefaultObject()->WeaponSlot))
						{
							if(AWeaponBase* Weapon = PlayerCharacter->AssignWeapon(WeaponClass))
							{
								PlayerCharacter->EquipStrongestWeapon();
								return true;
							}
						}else
						{
							//Drop weapon and buy new one?
						}
					}
				}
				
			}
		}
	}
	return false;
}

bool ACombatGameMode::SellItem(ADefaultPlayerCharacter* PlayerCharacter, EItemIdentifier ItemIdentifier)
{
	if(ItemPriceList.Contains(ItemIdentifier))
	{
		int32 ItemPrice = *ItemPriceList.Find(ItemIdentifier);
		if(ACombatPlayerState* CombatPlayerState = Cast<ACombatPlayerState>(PlayerCharacter->GetPlayerState()))
		{
			if(CombatPlayerState->GetPlayerMoney() >= ItemPrice)
			{

				if(ItemIdentifier == EItemIdentifier::LightShield || ItemIdentifier == EItemIdentifier::HeavyShield)
				{

					//Shield Purchase:
					
					
				}else
				{
					//Weapon purchase:
					if(TSubclassOf<AWeaponBase> WeaponClass = *WeaponIdTable.Find(ItemIdentifier))
					{
						if(AWeaponBase* Weapon = PlayerCharacter->AssignWeapon(WeaponClass))
						{
							PlayerCharacter->EquipStrongestWeapon();
							return true;
						}
					}
				}
				
			}
		}
	}
	return false;
}
