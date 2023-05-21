// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/InteractionComponent.h"
#include "BombPickupInteraction.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = Interacion, meta = (BlueprintSpawnableComponent))
class SNAILRELOADED_API UBombPickupInteraction : public UInteractionComponent
{
	GENERATED_BODY()

public:
	virtual void BeginFocus(APawn* Interactor) override;
	virtual void BeginInteract(APawn* Interactor) override;
	virtual void Interact(APawn* Interactor) override;
};
