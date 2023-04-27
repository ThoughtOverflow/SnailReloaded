// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Combat/CombatGameState.h"
#include "StandardCombatGameState.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API AStandardCombatGameState : public ACombatGameState
{
	GENERATED_BODY()

public:

	AStandardCombatGameState();

protected:

	virtual void OnPhaseExpired(EGamePhase ExpiredPhase) override;
	virtual void OnPhaseSelected(EGamePhase NewPhase) override;
	virtual void StartNewRound() override;
	
	
};
