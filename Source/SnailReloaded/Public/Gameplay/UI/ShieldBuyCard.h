// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/UI/ItemBuyCard.h"
#include "ShieldBuyCard.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API UShieldBuyCard : public UItemBuyCard
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentShieldHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxShieldHealth;
	
};
