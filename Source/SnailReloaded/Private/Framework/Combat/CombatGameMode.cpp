// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/CombatGameMode.h"

#include "Components/ArmoredHealthComponent.h"
#include "Components/HealthComponent.h"
#include "Framework/Combat/CombatPlayerState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"

ACombatGameMode::ACombatGameMode()
{
	
}

FShieldProperties* ACombatGameMode::FindShieldDataByType(EItemIdentifier ShieldIdentifier)
{
	for(auto& Shield : ShieldDataTable)
	{
		if(Shield.ShieldIdentifier == ShieldIdentifier) return &Shield;
	}
	return nullptr;
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
				if(ItemIdentifier == EItemIdentifier::LightShield || ItemIdentifier == EItemIdentifier::HeavyShield)
				{
					EItemIdentifier CurrentShield = PlayerCharacter->GetCurrentShieldType();
					if(CurrentShield == ItemIdentifier) return false;
					if(CurrentShield == EItemIdentifier::NullShield)
					{
						//Shield Purchase:
						if(FShieldProperties* ShieldData = FindShieldDataByType(ItemIdentifier))
						{
							//Remove monkey:
							CombatPlayerState->ChangePlayerMoney(-ItemPrice);
							PlayerCharacter->UpdateShieldProperties(*ShieldData);
						}
					}else
					{
						//Sell, then buy it
						if(FShieldProperties* ShieldData = FindShieldDataByType(ItemIdentifier))
						{
							if(ItemPriceList.Contains(CurrentShield))
							{
								//Remove monkey:
								int32 ReturnMoney = PlayerCharacter->CanSellCurrentShield() ? *ItemPriceList.Find(CurrentShield) : 0;
								CombatPlayerState->ChangePlayerMoney(-ItemPrice + ReturnMoney);
								PlayerCharacter->UpdateShieldProperties(*ShieldData);
							}
						}
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
								//Remove monkey:
								CombatPlayerState->ChangePlayerMoney(-ItemPrice);
								return true;
							}
						}else
						{
							//Drop weapon and buy new one?
							if(AWeaponBase* Weapon = PlayerCharacter->GetWeaponAtSlot(WeaponClass.GetDefaultObject()->WeaponSlot))
							{
								if(Weapon->ItemIdentifier == ItemIdentifier) return false;
								if(Weapon->CanSell())
								{

									//sell, then buy it.

									if(SellItem(PlayerCharacter, ItemIdentifier))
									{
											PlayerCharacter->AssignWeapon(WeaponClass);
											PlayerCharacter->EquipStrongestWeapon();
											//Remove monkey:
											CombatPlayerState->ChangePlayerMoney(-ItemPrice);
											return true;
									}
									
								}else
								{
									//Drop current, buy new one.
								}
							}
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
			//Check for shield:

			if(ItemIdentifier == EItemIdentifier::LightShield || ItemIdentifier == EItemIdentifier::HeavyShield)
			{
				if(PlayerCharacter->CanSellCurrentShield() && PlayerCharacter->GetCurrentShieldType() == ItemIdentifier)
				{
					//Sell it:
					CombatPlayerState->ChangePlayerMoney(ItemPrice);
					//set null shield.
					PlayerCharacter->UpdateShieldProperties(FShieldProperties());
					return true;
				}
				return false;	
			}
			
			
			EWeaponSlot CurrentSlot = PlayerCharacter->GetWeaponSlotByIdentifier(ItemIdentifier);
			if(CurrentSlot != EWeaponSlot::None)
			{
				if(AWeaponBase* Weapon = PlayerCharacter->GetWeaponAtSlot(CurrentSlot))
				{
					if(Weapon->CanSell())
					{
						PlayerCharacter->RemoveWeapon(CurrentSlot);
						CombatPlayerState->ChangePlayerMoney(ItemPrice);
						return true;
					}
				}
				
			}
		}
	}
	return false;
}
