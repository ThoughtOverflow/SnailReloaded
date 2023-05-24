// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/UI/Components/ColorSelector.h"

UColorSelector::UColorSelector()
{
	OnClicked.AddDynamic(this, &UColorSelector::OnColorSelected);
}

void UColorSelector::OnColorSelected()
{
	if(ACombatPlayerState* CombatPlayerState = GetOwningPlayer()->GetPlayerState<ACombatPlayerState>())
	{
		CombatPlayerState->SetPlayerColor(AssignedColor);
	}
}
