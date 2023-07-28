// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OutfitWidget.generated.h"

enum class EOutfitRarity : uint8;
/**
 * 
 */
UCLASS()
class SNAILRELOADED_API UOutfitWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly,	 BlueprintReadWrite)
	UTexture2D* OutfitThumbnail;
	UPROPERTY(EditDefaultsOnly,	 BlueprintReadWrite)
	FText OutfitName;
	UPROPERTY(EditDefaultsOnly,	 BlueprintReadWrite)
	FText OutfitGroup;
	UPROPERTY(EditDefaultsOnly,	 BlueprintReadWrite)
	FString OutfitId;
	UPROPERTY(EditDefaultsOnly,	 BlueprintReadWrite)
	bool bOutfitEquipped;
	UPROPERTY(EditDefaultsOnly,	 BlueprintReadWrite)
	EOutfitRarity Rarity;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
};
