// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Framework/Combat/CombatPlayerState.h"
#include "ColorSelector.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class SNAILRELOADED_API UColorSelector : public UButton
{
	GENERATED_BODY()

public:

	UColorSelector();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPlayerColor AssignedColor;

	UFUNCTION()
	void OnColorSelected();
	
};
