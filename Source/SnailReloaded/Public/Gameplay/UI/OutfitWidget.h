// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OutfitWidget.generated.h"

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

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
};
