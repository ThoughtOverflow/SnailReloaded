// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Components/PlayerHeaderComponent.h"

#include "Framework/Combat/CombatGameState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Gameplay/UI/PlayerHeaderWidget.h"

UPlayerHeaderComponent::UPlayerHeaderComponent()
{
	SetActive(true);
	SetHiddenInGame(false);
	SetWidgetSpace(EWidgetSpace::Screen);
}

void UPlayerHeaderComponent::Reload()
{
	if(UPlayerHeaderWidget* PlayerHeaderWidget = Cast<UPlayerHeaderWidget>(GetUserWidgetObject()))
	{
		if(ADefaultPlayerCharacter* DefaultPlayerCharacter = Cast<ADefaultPlayerCharacter>(GetOwner()))
		{
			PlayerHeaderWidget->SourcePlayer = DefaultPlayerCharacter;
			if(ACombatGameState* CombatGameState = Cast<ACombatGameState>(GetWorld()->GetGameState()))
			{
				for(auto& PlayerState : CombatGameState->PlayerArray)
				{
					if(PlayerState->GetPawn() == DefaultPlayerCharacter)
					{
						PlayerHeaderWidget->PlayerName = FText::FromString(PlayerState->GetPlayerName());
						break;
					}
				}
			}
		}
		
	}
}

void UPlayerHeaderComponent::BeginPlay()
{
	Super::BeginPlay();

	Reload();
	
}
