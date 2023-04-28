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

void AStandardCombatGameMode::InitializeCurrentGame()
{

	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
		StandardCombatGameState->DefuseTime = DefuseTime;
		StandardCombatGameState->PlantTime = PlantTime;
	}
	
	Super::InitializeCurrentGame();
}
