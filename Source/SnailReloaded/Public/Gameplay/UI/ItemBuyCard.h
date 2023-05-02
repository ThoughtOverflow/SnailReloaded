// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "Gameplay/Weapons/WeaponBase.h"
#include "ItemBuyCard.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API UItemBuyCard : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemIdentifier ItemIdentifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemDisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemDisplayPrice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor HoveredColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor OwningColor;

	UFUNCTION(BlueprintCallable)
	void OnPurchase();
	UFUNCTION(BlueprintCallable)
	void OnSell();
	
};
