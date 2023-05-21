// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PlayerHeaderComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = Custom, meta = (BlueprintSpawnableComponent))
class SNAILRELOADED_API UPlayerHeaderComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

	UPlayerHeaderComponent();

	UFUNCTION()
	void Reload();
	
protected:

	virtual void BeginPlay() override;
	
};
