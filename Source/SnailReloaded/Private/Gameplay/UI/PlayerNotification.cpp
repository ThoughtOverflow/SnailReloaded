// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/UI/PlayerNotification.h"

#include "Framework/Combat/CombatPlayerController.h"

void UPlayerNotification::NotificationExpired()
{
	Cast<ACombatPlayerController>(GetOwningPlayer())->RemovePlayerNotification(this);
}

void UPlayerNotification::NativeConstruct()
{
	Super::NativeConstruct();

	GetOwningPlayer()->GetWorldTimerManager().SetTimer(ExpiryHandle, this, &UPlayerNotification::NotificationExpired, NotificationTime);
}
