// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameNotification.generated.h"

UENUM()
enum class ENotificationType : uint8
{
	PrepPhase,
	BombPlanted,
	RoundLost,
	RoundWon
};

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API UGameNotification : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float NotificationTime;
	
};
