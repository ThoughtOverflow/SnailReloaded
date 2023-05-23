// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/UI/TeamSelectionWidget.h"

#include "Framework/Combat/CombatPlayerState.h"

FLinearColor UTeamSelectionWidget::GetColorByEnum(EPlayerColor Color)
{
	if(Color == EPlayerColor::None) return FLinearColor::White;
	return *ColorMap.Find(Color);
}
