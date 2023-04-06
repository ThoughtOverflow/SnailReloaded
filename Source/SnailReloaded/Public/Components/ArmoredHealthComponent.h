// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "ArmoredHealthComponent.generated.h"


/**
 * 
 */
UCLASS( ClassGroup=(HealthSystem), meta=(BlueprintSpawnableComponent) )
class SNAILRELOADED_API UArmoredHealthComponent : public UHealthComponent
{
	GENERATED_BODY()

public:

	UArmoredHealthComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shield Properties")
	float ShieldHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shield Properties")
	float ShieldDamageReduction;
	
protected:

	virtual FDamageResponse ChangeObjectHealth(FDamageRequest DamageRequest) override;
	
public:
	
	
};
