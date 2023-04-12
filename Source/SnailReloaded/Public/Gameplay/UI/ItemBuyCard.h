// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemBuyCard.generated.h"

enum class EItemIdentifier;
/**
 * 
 */
UCLASS()
class SNAILRELOADED_API UItemBuyCard : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EItemIdentifier ItemIdentifier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText ItemDisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ItemDisplayPrice;

	UFUNCTION(BlueprintCallable)
	void OnPurchase();
	
};
