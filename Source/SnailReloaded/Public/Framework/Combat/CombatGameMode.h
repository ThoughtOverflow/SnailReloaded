// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/DefaultGameMode.h"
#include "CombatGameMode.generated.h"

struct FDamageRequest;
struct FDamageResponse;
/**
 * 
 */
UCLASS()
class SNAILRELOADED_API ACombatGameMode : public ADefaultGameMode
{
	GENERATED_BODY()

public:

	ACombatGameMode();

	

protected:

	
	
public:


	UFUNCTION(BlueprintCallable)
	FDamageResponse ChangeObjectHealth(FDamageRequest DamageRequest);
	
};
