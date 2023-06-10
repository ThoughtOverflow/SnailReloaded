// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Gadgets/Gadget.h"
#include "Components/SphereComponent.h"
#include "Turret.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API ATurret : public AGadget
{
	GENERATED_BODY()

	ATurret();
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USphereComponent* SightRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ADefaultPlayerCharacter* CurrentTarget;


	UFUNCTION(BlueprintCallable)
	void FireTurret(ADefaultPlayerCharacter* Target);

	virtual void Tick(float DeltaTime) override;
	
};
