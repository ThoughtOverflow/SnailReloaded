// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/UI/SkinOpeningWidget.h"

#include "Framework/DefaultPlayerState.h"


void USkinOpeningWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(ADefaultPlayerState* PlayerState = GetOwningPlayer()->GetPlayerState<ADefaultPlayerState>())
	{
		//query account data when widget is created.
		PlayerState->API_GetPlayerAccountData();
	}
}
