// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/Standard/StandardCombatGameMode.h"

#include "Framework/Combat/Standard/StandardCombatGameState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Gameplay/Weapons/Bomb.h"

AStandardCombatGameMode::AStandardCombatGameMode()
{
	
}

void AStandardCombatGameMode::BeginPlanting(ADefaultPlayerCharacter* PlantingPlayer)
{
	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
		if(StandardCombatGameState->GetCurrentGamePhase().GamePhase == EGamePhase::ActiveGame)
		{
			StandardCombatGameState->SetPlayerPlanting(PlantingPlayer, true);
		}
	}
}

void AStandardCombatGameMode::EndPlanting(ADefaultPlayerCharacter* PlantingPlayer)
{
	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
		if(StandardCombatGameState->GetCurrentGamePhase().GamePhase == EGamePhase::ActiveGame)
		{
			StandardCombatGameState->SetPlayerPlanting(PlantingPlayer, false);
		}
	}
}

void AStandardCombatGameMode::BeginDefuse(ADefaultPlayerCharacter* PlantingPlayer)
{
	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
		if(StandardCombatGameState->GetCurrentGamePhase().GamePhase == EGamePhase::ActiveGame)
		{
			StandardCombatGameState->SetPlayerDefusing(PlantingPlayer, true);
		}
	}
}

void AStandardCombatGameMode::EndDefuse(ADefaultPlayerCharacter* PlantingPlayer)
{
	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
		if(StandardCombatGameState->GetCurrentGamePhase().GamePhase == EGamePhase::ActiveGame)
		{
			StandardCombatGameState->SetPlayerDefusing(PlantingPlayer, false);
		}
	}
}

void AStandardCombatGameMode::PlantBomb(ADefaultPlayerCharacter* Planter)
{
	//plant bomb - change phases.
	if(BombActor)
	{
		if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
		{
			FGamePhase PostPlantPhase;
			if(GetGamePhaseByType(EGamePhase::PostPlant, PostPlantPhase))
			{
				StandardCombatGameState->PlantedBomb = GetWorld()->SpawnActor<ABomb>(BombActor, Planter->GetActorLocation() + Planter->GetActorRotation().Vector() * 100.f, FRotator::ZeroRotator);
				StandardCombatGameState->PlantedBomb->PlantBomb();
				Planter->SetHasBomb(false);
			}
		}
	}
}

void AStandardCombatGameMode::DefuseBomb(ADefaultPlayerCharacter* Defuser)
{
	//stop bomb - end game.
}

bool AStandardCombatGameMode::SwapTeams()
{
	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
			
		for(auto& PlayerState : StandardCombatGameState->PlayerArray)
		{
			if(ACombatPlayerState* CombatPlayerState = Cast<ACombatPlayerState>(PlayerState))
			{
				switch (CombatPlayerState->GetTeam()) {
				case EGameTeams::None: break;
				case EGameTeams::TeamA: CombatPlayerState->SetTeam(EGameTeams::TeamB); break;
				case EGameTeams::TeamB: CombatPlayerState->SetTeam(EGameTeams::TeamA); break;
				default: ;
				}
			}
		}
		return true;
		
	}
	return false;
}


void AStandardCombatGameMode::InitializeCurrentGame()
{

	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
		StandardCombatGameState->DefuseTime = DefuseTime;
		StandardCombatGameState->PlantTime = PlantTime;
		StandardCombatGameState->NumberOfRounds = MaxRounds;
	}
	
	Super::InitializeCurrentGame();
}

void AStandardCombatGameMode::StartRound()
{
	Super::StartRound();

	if(ACombatGameState* CombatGameState = GetGameState<ACombatGameState>())
	{
		if(CombatGameState->GetCurrentRound() < MaxRounds)
		{
			//Check for the team swap:
			if(CombatGameState->GetCurrentRound() == FMath::CeilToInt(MaxRounds / 2.f))
			{
				SwapTeams();
			}
		}
	}
	
}
