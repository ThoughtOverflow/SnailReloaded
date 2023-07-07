// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/UI/OutfitWidget.h"

#include "Framework/DefaultPlayerState.h"

FReply UOutfitWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		//Equip thingmajig.
		if(ADefaultPlayerState* DefaultPlayerState = GetOwningPlayer()->GetPlayerState<ADefaultPlayerState>())
		{
			DefaultPlayerState->API_EquipOutfit(OutfitId);
		}
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
