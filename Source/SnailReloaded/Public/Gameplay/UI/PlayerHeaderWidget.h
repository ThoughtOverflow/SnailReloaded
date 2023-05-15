// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHeaderWidget.generated.h"

class ADefaultPlayerCharacter;
/**
 * 
 */
UCLASS()
class SNAILRELOADED_API UPlayerHeaderWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	ADefaultPlayerCharacter* SourcePlayer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player properties")
	FText PlayerName = FText::FromString("None");
	
};
