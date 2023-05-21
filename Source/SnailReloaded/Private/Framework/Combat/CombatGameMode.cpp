// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/CombatGameMode.h"

#include "Components/ArmoredHealthComponent.h"
#include "Components/HealthComponent.h"
#include "Framework/Combat/CombatGameState.h"
#include "Framework/Combat/CombatPlayerState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"

FGamePhase::FGamePhase(): GamePhase(EGamePhase::None), PhaseTimeSeconds(0)
{
}

ACombatGameMode::ACombatGameMode()
{
	bMatchEnded = false;
	OvertimeScoreDifference = 1;
	MaxOvertimeRounds = 1;
}

FShieldProperties* ACombatGameMode::FindShieldDataByType(EItemIdentifier ShieldIdentifier)
{
	for(auto& Shield : ShieldDataTable)
	{
		if(Shield.ShieldIdentifier == ShieldIdentifier)
		{
			//Set max to default.
			Shield.ShieldMaxHealth = Shield.ShieldHealth;
			return &Shield;
		}
	}
	return nullptr;
}

void ACombatGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	if(ACombatPlayerState* PlayerState = NewPlayer->GetPlayerState<ACombatPlayerState>())
	{
		GetGameState<ACombatGameState>()->AddGamePlayer(PlayerState);
	}
}

void ACombatGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if(ACombatPlayerState* PlayerState = Exiting->GetPlayerState<ACombatPlayerState>())
	{
		GetGameState<ACombatGameState>()->RemoveGamePlayer(PlayerState);
	}
}

FDamageResponse ACombatGameMode::ChangeObjectHealth(const FDamageRequest& DamageRequest)
{
	if(DamageRequest.SourceActor)
	{
		if(UHealthComponent* TargetHealthComponent = Cast<UHealthComponent>(DamageRequest.TargetActor->GetComponentByClass(UHealthComponent::StaticClass())))
		{
			if(UHealthComponent* SourceHealthComponent = Cast<UHealthComponent>(DamageRequest.SourceActor->GetComponentByClass(UHealthComponent::StaticClass())))
			{
				FDamageRequest ModifiedRequest = DamageRequest;
				if(DamageRequest.DeltaDamage < 0.f)
				{
					//Only apply modifiers for damage, not heal.
					ModifiedRequest.DeltaDamage *= SourceHealthComponent->GetDamageMultiplierForTarget(TargetHealthComponent);
				}
				const FDamageResponse Response = TargetHealthComponent->ChangeObjectHealth(ModifiedRequest);
				return Response;
			}
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
					if(CurrentShield == EItemIdentifier::NullShield)
					{
						//Shield Purchase:
						if(FShieldProperties* ShieldData = FindShieldDataByType(ItemIdentifier))
						{
							//Raw buy
							CombatPlayerState->ChangePlayerMoney(-ItemPrice);
							PlayerCharacter->UpdateShieldProperties(*ShieldData);
							PlayerCharacter->StoreCurrentShieldData();
							PlayerCharacter->SetCanSellCurrentShield(true);
						}
					}else
					{
						//Sell, then buy
						if(FShieldProperties* ShieldData = FindShieldDataByType(ItemIdentifier))
						{
							if(ItemPriceList.Contains(CurrentShield))
							{
								
								//Remove monkey:
								int32 ReturnMoney = 0;
								if(PlayerCharacter->CanSellCurrentShield())
								{
									ReturnMoney = *ItemPriceList.Find(CurrentShield);
									
								}else
								{
									PlayerCharacter->StoreCurrentShieldData();
								}
								CombatPlayerState->ChangePlayerMoney(-ItemPrice + ReturnMoney);
								PlayerCharacter->UpdateShieldProperties(*ShieldData);
								PlayerCharacter->SetCanSellCurrentShield(true);
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
					//Check for restore shield:
					if(PlayerCharacter->HasStoredShield())
					{
						PlayerCharacter->RevertToPreviousShield();
					}else
					{
						PlayerCharacter->UpdateShieldProperties(FShieldProperties());
					}
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

void ACombatGameMode::InitializeCurrentGame()
{
	if(ACombatGameState* CombatGameState = GetGameState<ACombatGameState>())
	{
		if(GamePhases.Num() > 0)
		{
			CombatGameState->InitialPlayerMoney = PlayerStartMoney;
			CombatGameState->StartNewRound();
			CombatGameState->CurrentGameInitialized();
		}
	}
}

bool ACombatGameMode::GetGamePhaseByType(EGamePhase Phase, FGamePhase& RefGamePhase)
{
	if(AllowedPhases.Contains(Phase))
	{
		for(int i=0; i<GamePhases.Num(); i++)
		{
			if(GamePhases[i].GamePhase == Phase)
			{
				RefGamePhase = GamePhases[i];
				return true;
			}
		}
	}
	return false;
}

void ACombatGameMode::StartRound()
{
	if(ACombatGameState* CombatGameState = GetGameState<ACombatGameState>())
	{
		if(GamePhases.Num() > 0)
		{
			if (CombatGameState->GetCurrentRound() < MaxRounds && !bMatchEnded)
			{
				CombatGameState->SetCurrentRound(CombatGameState->GetCurrentRound() + 1);
				CombatGameState->SetCurrentGamePhase(GamePhases[0]);
			}else
			{
				if(bAllowOvertime)
				{
					//Do Overtime stuff.
				}else
				{
					if(!bMatchEnded)
					{
						EndMatch();
					}
				}
			}
		}
	}

}

void ACombatGameMode::EndMatch()
{
	//Override - Match end results.
	if(!bMatchEnded) bMatchEnded = true;
	
}

bool ACombatGameMode::IsMatchOver()
{
	return bMatchEnded;
}

void ACombatGameMode::ProcessPlayerDeath(ACombatPlayerState* PlayerState)
{
	//Death logic - override.
}
