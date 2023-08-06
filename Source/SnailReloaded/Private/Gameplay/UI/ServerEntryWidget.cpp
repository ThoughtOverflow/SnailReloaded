// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/UI/ServerEntryWidget.h"

#include "Framework/SnailGameInstance.h"

void UServerEntryWidget::OnJoinRequest()
{
	if(USnailGameInstance* SnailGameInstance = Cast<USnailGameInstance>(GetGameInstance()))
	{
		SnailGameInstance->JoinFoundSession(SessionName, SearchResult);
	}
}
