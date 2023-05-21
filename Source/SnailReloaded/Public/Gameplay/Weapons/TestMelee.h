// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Weapons/WeaponBase.h"
#include "TestMelee.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API ATestMelee : public AWeaponBase
{
	GENERATED_BODY()

public:
	
	ATestMelee();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UNiagaraComponent* TrailParticleComponent;

public:

	virtual void OnFireAnimPlayed() override;
	
};
