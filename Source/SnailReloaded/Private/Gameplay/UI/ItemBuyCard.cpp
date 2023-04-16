// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/UI/ItemBuyCard.h"

#include "Gameplay/Player/DefaultPlayerCharacter.h"

void UItemBuyCard::OnPurchase()
{
	if(GetOwningPlayer() && GetOwningPlayer()->GetPawn())
	{
		if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(GetOwningPlayer()->GetPawn()))
		{
			PlayerCharacter->TryPurchaseItem(ItemIdentifier);
		}
	}
}

void UItemBuyCard::OnSell()
{
	if(GetOwningPlayer() && GetOwningPlayer()->GetPawn())
	{
		if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(GetOwningPlayer()->GetPawn()))
		{
			PlayerCharacter->TrySellItem(ItemIdentifier);
		}
	}
}
