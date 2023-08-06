// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/UI/PlayerHud.h"

#include "Gameplay/UI/PlayerNotification.h"

UPlayerNotification* UPlayerHud::CreatePlayerNotification(const FText& NotificationText, const FLinearColor& NotificationColor,
                                          float Time)
{
	if(PlayerNotificationWrapper && PlayerNotificationClass)
	{
		UPlayerNotification* PlayerNotification = CreateWidget<UPlayerNotification>(GetOwningPlayer(), PlayerNotificationClass);
		PlayerNotification->NotificationText = NotificationText;
		PlayerNotification->NotificationColor = NotificationColor;
		PlayerNotification->NotificationTime = Time;
		PlayerNotificationWrapper->AddChildToVerticalBox(PlayerNotification);
		return PlayerNotification;
	}
	return nullptr;
}
