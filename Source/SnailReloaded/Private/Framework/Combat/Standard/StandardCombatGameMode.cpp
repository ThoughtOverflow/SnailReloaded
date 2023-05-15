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
			if(StandardCombatGameState->IsPlayerPlanting(PlantingPlayer))
			{
				StandardCombatGameState->SetPlayerPlanting(PlantingPlayer, false);
			}
		}
	}
}

void AStandardCombatGameMode::BeginDefuse(ADefaultPlayerCharacter* PlantingPlayer)
{
	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
		if(StandardCombatGameState->GetCurrentGamePhase().GamePhase == EGamePhase::PostPlant)
		{
			StandardCombatGameState->SetPlayerDefusing(PlantingPlayer, true);
		}
	}
}

void AStandardCombatGameMode::EndDefuse(ADefaultPlayerCharacter* PlantingPlayer)
{
	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
		if(StandardCombatGameState->GetCurrentGamePhase().GamePhase == EGamePhase::PostPlant)
		{
			if(StandardCombatGameState->IsPlayerDefusing(PlantingPlayer))
			{
				StandardCombatGameState->SetPlayerDefusing(PlantingPlayer, false);
			}
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
	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
		if(StandardCombatGameState->PlantedBomb)
		{
			StandardCombatGameState->PlantedBomb->DefuseBomb();
		}
	}
}

bool AStandardCombatGameMode::SwapTeamForPlayer(ACombatPlayerState* PlayerState)
{
	switch (PlayerState->GetTeam()) {
	case EGameTeams::None: break;
	case EGameTeams::TeamA: PlayerState->SetTeam(EGameTeams::TeamB); return true;
	case EGameTeams::TeamB: PlayerState->SetTeam(EGameTeams::TeamA); return true;
	default: ;
	}
	return false;
}


bool AStandardCombatGameMode::SwapSides()
{
	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
		switch (StandardCombatGameState->GetSideByTeam(EGameTeams::TeamA)) {
		case EBombTeam::None: break;
		case EBombTeam::Attacker: StandardCombatGameState->SetSideOfTeam(EGameTeams::TeamA, EBombTeam::Defender); break;
		case EBombTeam::Defender: StandardCombatGameState->SetSideOfTeam(EGameTeams::TeamA, EBombTeam::Attacker); break;
		default: ;
		}
		switch (StandardCombatGameState->GetSideByTeam(EGameTeams::TeamB)) {
		case EBombTeam::None: break;
		case EBombTeam::Attacker: StandardCombatGameState->SetSideOfTeam(EGameTeams::TeamB, EBombTeam::Defender); break;
		case EBombTeam::Defender: StandardCombatGameState->SetSideOfTeam(EGameTeams::TeamB, EBombTeam::Attacker); break;
		default: ;
		}
		return true;
	}
	return false;
}

void AStandardCombatGameMode::EndMatch()
{
	Super::EndMatch();

	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
		int32 AScore = StandardCombatGameState->GetScoreForTeam(EGameTeams::TeamA);
		int32 BScore = StandardCombatGameState->GetScoreForTeam(EGameTeams::TeamB);
		
		if(AScore == BScore)
		{
			//Handle tie
			UE_LOG(LogTemp, Warning, TEXT("TIE"));
			
		}else if(AScore > BScore)
		{
			//TeamA Win
			UE_LOG(LogTemp, Warning, TEXT("TeamA WIN!"));
		}else
		{
			//TeamB Win
			UE_LOG(LogTemp, Warning, TEXT("TeamB WIN!"));
		}
	}
	
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
				SwapSides();
			}
		}
	}
	
}

void AStandardCombatGameMode::ProcessPlayerDeath(ACombatPlayerState* PlayerState)
{
	Super::ProcessPlayerDeath(PlayerState);

	//Check for any alive players - end round if team's dead.

	if(AStandardCombatGameState* StandardCombatGameState = GetGameState<AStandardCombatGameState>())
	{
		StandardCombatGameState->CheckForAlivePlayers();
	}
}
