// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Components/BombPickupInteraction.h"

#include "Framework/Combat/Standard/StandardCombatGameState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"

void UBombPickupInteraction::BeginFocus(APawn* Interactor)
{

	if(AStandardCombatGameState* StandardCombatGameState = Cast<AStandardCombatGameState>(GetWorld()->GetGameState()))
	{
		if(ACombatPlayerState* CombatPlayerState = Interactor->GetController()->GetPlayerState<ACombatPlayerState>())
		{
			if(StandardCombatGameState->GetSideByTeam(CombatPlayerState->GetTeam()) == EBombTeam::Attacker)
			{
				Super::BeginFocus(Interactor);
			}
		}
		
	}
}

void UBombPickupInteraction::BeginInteract(APawn* Interactor)
{
	if(AStandardCombatGameState* StandardCombatGameState = Cast<AStandardCombatGameState>(GetWorld()->GetGameState()))
	{
		if(ACombatPlayerState* CombatPlayerState = Interactor->GetController()->GetPlayerState<ACombatPlayerState>())
		{
			if(StandardCombatGameState->GetSideByTeam(CombatPlayerState->GetTeam()) == EBombTeam::Attacker)
			{
				Super::BeginInteract(Interactor);
			}
		}
		
	}
}

void UBombPickupInteraction::Interact(APawn* Interactor)
{
	if(AStandardCombatGameState* StandardCombatGameState = Cast<AStandardCombatGameState>(GetWorld()->GetGameState()))
	{
		if(ACombatPlayerState* CombatPlayerState = Interactor->GetController()->GetPlayerState<ACombatPlayerState>())
		{
			if(StandardCombatGameState->GetSideByTeam(CombatPlayerState->GetTeam()) == EBombTeam::Attacker)
			{
				Super::Interact(Interactor);
			}
		}
		
	}
}
