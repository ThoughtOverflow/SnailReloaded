// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/UI/InteractionWidget.h"
#include "WeaponPickupWidget.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API UWeaponPickupWidget : public UInteractionWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText WeaponName;
	
};
