// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API UDamageIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	AActor* SourceActor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Angle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector InitialSourceLocation;

	
	
};
