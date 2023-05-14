// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "Framework/Combat/Standard/StandardCombatGameState.h"
#include "GameFramework/PlayerStart.h"
#include "TeamPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API ATeamPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn options")
	EBombTeam AssignedTeam = EBombTeam::None;
	
};
