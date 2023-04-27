// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/Standard/StandardCombatGameState.h"

AStandardCombatGameState::AStandardCombatGameState()
{
	
}

void AStandardCombatGameState::OnPhaseExpired()
{
	Super::OnPhaseExpired();

	switch (CurrentGamePhase.GamePhase) {
	case EGamePhase::None: break;
	case EGamePhase::Preparation: SelectNewPhase(EGamePhase::ActiveGame); break;
	case EGamePhase::ActiveGame: SelectNewPhase(EGamePhase::EndPhase); break;
	case EGamePhase::PostPlant: break;
	case EGamePhase::EndPhase: StartNewRound();
	default: ;
	}
}

void AStandardCombatGameState::OnPhaseSelected(EGamePhase NewPhase)
{
	Super::OnPhaseSelected(NewPhase);
	if(NewPhase == EGamePhase::EndPhase)
	{
		//Do Scoring - round finished.
	}
}
