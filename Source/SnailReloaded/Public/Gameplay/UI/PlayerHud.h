// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "PlayerHud.generated.h"

/**
 * 
 */

enum class EPlayerColor : uint8;
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float PlayerShieldHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	bool bIsReloading;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Planting")
	bool bShowPlantHint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Planting")
	bool bShowDefuseHint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	UOverlay* NotificationWrapper;
	
	
	
};
