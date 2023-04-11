// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHud.generated.h"

/**
 * 
 */

class ACombatPlayerController;

UCLASS()
class SNAILRELOADED_API UPlayerHud : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	float PlayerHealthPercentage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FText CurrentWeaponName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentClipAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentTotalAmmo;
};
