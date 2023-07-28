// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerNotification.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API UPlayerNotification : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText NotificationText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FLinearColor NotificationColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float NotificationTime;

	UFUNCTION()
	void NotificationExpired();
	UPROPERTY()
	FTimerHandle ExpiryHandle;

	virtual void NativeConstruct() override;
	
	
};
