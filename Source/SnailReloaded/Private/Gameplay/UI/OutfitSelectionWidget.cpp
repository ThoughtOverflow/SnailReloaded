// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/UI/OutfitSelectionWidget.h"

#include "Framework/DefaultPlayerState.h"


void UOutfitSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(ADefaultPlayerState* PlayerState = GetOwningPlayer()->GetPlayerState<ADefaultPlayerState>())
	{
		//query inv data when widget is created.
		PlayerState->API_GetPlayerInventoryData();
	}
}
