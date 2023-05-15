// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/Standard/StandardCombatGameState.h"

#include "Framework/Combat/CombatPlayerController.h"
#include "Framework/Combat/CombatPlayerState.h"
#include "Framework/Combat/Standard/StandardCombatGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Gameplay/Weapons/Bomb.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "World/Objects/TeamPlayerStart.h"

AStandardCombatGameState::AStandardCombatGameState()
{
	TeamAScore = 0;
	TeamBScore = 0;
	RoundEndResult = ERoundEndResult::None;
	TeamASide = EBombTeam::Attacker;
	TeamBSide = EBombTeam::Defender;
}

void AStandardCombatGameState::OnPhaseExpired(EGamePhase ExpiredPhase)
{
	Super::OnPhaseExpired(ExpiredPhase);

	if(ExpiredPhase == EGamePhase::Preparation)
	{
		//Set all players' weapons sell property to false.
		for(TObjectPtr<APlayerState> PlayerState : PlayerArray)
		{
			if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(PlayerState->GetPawn()))
			{
				if(PlayerCharacter->PrimaryWeapon) PlayerCharacter->PrimaryWeapon->SetCanSell(false);
				if(PlayerCharacter->SecondaryWeapon) PlayerCharacter->SecondaryWeapon->SetCanSell(false);
				PlayerCharacter->PlayerHealthComponent->SetCanSell(false);
			}
		}
	}else if(ExpiredPhase == EGamePhase::ActiveGame)
	{
		//Timer ran out.
		RoundEndResult = ERoundEndResult::OutOfTime;
	}

	switch (CurrentGamePhase.GamePhase) {
	case EGamePhase::None: break;
	case EGamePhase::Preparation: SelectNewPhase(EGamePhase::ActiveGame); break;
	case EGamePhase::ActiveGame: SelectNewPhase(EGamePhase::EndPhase); break;
	case EGamePhase::PostPlant: SelectNewPhase(EGamePhase::EndPhase); break;
	case EGamePhase::EndPhase: StartNewRound();
	default: ;
	}
}

void AStandardCombatGameState::OnPhaseSelected(EGamePhase NewPhase)
{
	Super::OnPhaseSelected(NewPhase);
	if(NewPhase == EGamePhase::EndPhase)
	{
		
		if(PlantedBomb)
		{
			ExplodeBomb();
		}
		
		//Do team scoring - round finished.
		HandleTeamScoring();
		
	}
	//Update plant hint graphic.
	for(TObjectPtr<APlayerState> PlayerState : PlayerArray)
	{
		if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(PlayerState->GetPawn()))
		{
			PlayerCharacter->CheckPlantRequirements();
		}
	}
}

void AStandardCombatGameState::StartNewRound()
{
	Super::StartNewRound();

	//Only restart if the game isn't already over:
	if(AStandardCombatGameMode* StandardCombatGameMode = Cast<AStandardCombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if(StandardCombatGameMode->IsMatchOver()) return;
	}
	
	//Reset win result;
	RoundEndResult = ERoundEndResult::None;
	RespawnPlayers();
	
	//Give bomb to random player;
	if(HasAuthority())
	{

		//Remove all bombs:
		for(auto& PlayerState : PlayerArray)
		{
			if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(PlayerState->GetPawn()))
			{
				PlayerCharacter->SetHasBomb(false);
			}
		}
		
		TArray<ACombatPlayerState*> TeamPlayers = GetAllPlayersOfTeam(GetTeamBySide(EBombTeam::Attacker));
		if(TeamPlayers.Num() > 0)
		{
			int32 randPlayerIndex = FMath::RandRange(0, TeamPlayers.Num() - 1);
			if(TeamPlayers[randPlayerIndex])
			{
				if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(TeamPlayers[randPlayerIndex]->GetPawn()))
				{
					PlayerCharacter->SetHasBomb(true);
				}
			}
		}
	}
	
}

void AStandardCombatGameState::ExplodeBomb()
{
	if(PlantedBomb)
	{
		if(IsSomeoneDefusing() && IsPlayerDefusing(LatestBombInteractor))
		{
			LatestBombInteractor->TryStopPlanting();
		}
		PlantedBomb->ExplodeBomb();
		PlantedBomb->Destroy();
		PlantedBomb = nullptr;
	}
	//Set win type to check post-report;
	RoundEndResult = ERoundEndResult::BombExplode;
}


void AStandardCombatGameState::OnRep_PlantDefuse()
{
	if(LatestBombInteractor->GetController() && LatestBombInteractor->GetController()->IsLocalController())
	{
		if(IsSomeonePlanting() || IsSomeoneDefusing())
		{
			LatestBombInteractor->GetController()->SetIgnoreMoveInput(true);
		}else
		{
			LatestBombInteractor->GetController()->ResetIgnoreMoveInput();
		}
		
	}
	
	if(IsSomeonePlanting())
	{
		GetWorldTimerManager().SetTimer(PlantTimer, this, &AStandardCombatGameState::PlantTimerCallback, PlantTime);
		
	}else
	{
		GetWorldTimerManager().ClearTimer(PlantTimer);
	}

	if(IsSomeoneDefusing())
	{
		GetWorldTimerManager().SetTimer(DefuseTimer, this, &AStandardCombatGameState::DefuseTimerCallback, DefuseTime);
	}else
	{
		GetWorldTimerManager().ClearTimer(DefuseTimer);
	}
}

void AStandardCombatGameState::PlantTimerCallback()
{
	//Plant the bomb:
	if(AStandardCombatGameMode* CombatGameMode = Cast<AStandardCombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		CombatGameMode->PlantBomb(LatestBombInteractor);
	}
}

void AStandardCombatGameState::DefuseTimerCallback()
{
	//defuse the bomb:
	if(HasAuthority())
	{
		if(AStandardCombatGameMode* StandardCombatGameMode = Cast<AStandardCombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			StandardCombatGameMode->DefuseBomb(LatestBombInteractor);
		}else
		{
			UE_LOG(LogTemp, Error, TEXT("YOMOM"));
		}
	}
}

void AStandardCombatGameState::RespawnPlayers()
{
	if(HasAuthority())
	{
		for(TObjectPtr<APlayerState> PlayerState : PlayerArray)
		{
			if(ACombatPlayerController* PlayerController = Cast<ACombatPlayerController>(PlayerState->GetPlayerController()))
			{
				if(ACombatPlayerState* CombatPlayerState = Cast<ACombatPlayerState>(PlayerState))
				{
					TArray<ATeamPlayerStart*> TeamSpecStart = GetPlayerStartsByTeam(GetSideByTeam(CombatPlayerState->GetTeam()));

					checkf(TeamSpecStart.Num() != 0, TEXT("No team specific player spawns exist! - quitting"));

					ATeamPlayerStart* RandStart = TeamSpecStart[FMath::RandRange(0, TeamSpecStart.Num() - 1)];
					ADefaultPlayerCharacter* CurrentCharacter = Cast<ADefaultPlayerCharacter>(PlayerController->GetPawn());
					if(!CurrentCharacter && PlayerCharacterClass)
					{
						PlayerController->ShowDeathScreen(false);
						CurrentCharacter = GetWorld()->SpawnActor<ADefaultPlayerCharacter>(PlayerCharacterClass, RandStart->GetActorLocation(), RandStart->GetActorRotation());
						PlayerController->Possess(CurrentCharacter);
					}
					CurrentCharacter->SetActorLocation(RandStart->GetActorLocation());
					PlayerController->SetRespawnRotation(RandStart->GetActorRotation());
					CurrentCharacter->BlockPlayerInputs(false);
				}
			}
		}
	}
}

void AStandardCombatGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStandardCombatGameState, DefuseTime);
	DOREPLIFETIME(AStandardCombatGameState, PlantTime);
	DOREPLIFETIME(AStandardCombatGameState, bIsPlayerDefusing);
	DOREPLIFETIME(AStandardCombatGameState, bIsPlayerPlanting);
	DOREPLIFETIME(AStandardCombatGameState, LatestBombInteractor);
	DOREPLIFETIME(AStandardCombatGameState, NumberOfRounds);
	DOREPLIFETIME(AStandardCombatGameState, TeamASide);
	DOREPLIFETIME(AStandardCombatGameState, TeamBSide);
	DOREPLIFETIME(AStandardCombatGameState, TeamAScore);
	DOREPLIFETIME(AStandardCombatGameState, TeamBScore);
}

void AStandardCombatGameState::HandleTeamScoring()
{
	EGameTeams TeamToScore = EGameTeams::None;
	if(RoundEndResult == ERoundEndResult::None) return;
	if(RoundEndResult == ERoundEndResult::AttackersKilled || RoundEndResult == ERoundEndResult::BombDefuse || RoundEndResult == ERoundEndResult::OutOfTime)
	{
		TeamToScore = GetTeamBySide(EBombTeam::Defender);
	}else if(RoundEndResult == ERoundEndResult::DefendersKilled || RoundEndResult == ERoundEndResult::BombExplode)
	{
		TeamToScore = GetTeamBySide(EBombTeam::Attacker);
	}
	ChangeScoreForTeam(TeamToScore, 1);
}

void AStandardCombatGameState::SetPlayerPlanting(ADefaultPlayerCharacter* Player, bool bPlanting)
{
	if(HasAuthority() && bIsPlayerPlanting != bPlanting)
	{
		LatestBombInteractor = Player;
		bIsPlayerPlanting = bPlanting;
		OnRep_PlantDefuse();
	}
}

void AStandardCombatGameState::SetPlayerDefusing(ADefaultPlayerCharacter* Player, bool bDefusing)
{
	if(HasAuthority() && bIsPlayerDefusing != bDefusing)
	{
		LatestBombInteractor = Player;
		bIsPlayerDefusing = bDefusing;
		OnRep_PlantDefuse();
	}
}

bool AStandardCombatGameState::IsSomeonePlanting()
{
	return bIsPlayerPlanting;
}

bool AStandardCombatGameState::IsSomeoneDefusing()
{
	return bIsPlayerDefusing;
}

bool AStandardCombatGameState::IsPlayerPlanting(ADefaultPlayerCharacter* Player)
{
	return bIsPlayerDefusing && LatestBombInteractor == Player;
}

bool AStandardCombatGameState::IsPlayerDefusing(ADefaultPlayerCharacter* Player)
{
	return bIsPlayerPlanting && LatestBombInteractor == Player;
}

void AStandardCombatGameState::OnBombPlanted()
{
	if(HasAuthority())
	{
		SetPlayerDefusing(LatestBombInteractor, false);
		SetPlayerPlanting(LatestBombInteractor, false);
		SelectNewPhase(EGamePhase::PostPlant);
	}
}

void AStandardCombatGameState::OnBombDefused()
{
	if(HasAuthority())
	{
		SetPlayerDefusing(LatestBombInteractor, false);
		SetPlayerPlanting(LatestBombInteractor, false);
		if(PlantedBomb)
		{
			PlantedBomb->Destroy();
		}
		SelectNewPhase(EGamePhase::EndPhase);
	}
}

float AStandardCombatGameState::GetBombActionTimeRemaining()
{
	if(IsSomeonePlanting())
	{
		return GetWorldTimerManager().GetTimerElapsed(PlantTimer);
	}
	if(IsSomeoneDefusing())
	{
		return GetWorldTimerManager().GetTimerElapsed(DefuseTimer);
	}
	return 0.f;
}

void AStandardCombatGameState::SetScoreForTeam(EGameTeams Team, int32 NewScore)
{
	if(HasAuthority())
	{
		switch (Team) { case EGameTeams::None: break;
		case EGameTeams::TeamA: TeamAScore = NewScore; break;
		case EGameTeams::TeamB: TeamBScore = NewScore; break;
		default: ;
		}

		if(GetScoreForTeam(Team) == FMath::CeilToInt(NumberOfRounds / 2.f))
		{
			//Score diff is already at max - End match. - //Note overtime handles by itself.
			if(AStandardCombatGameMode* StandardCombatGameMode = Cast<AStandardCombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				StandardCombatGameMode->EndMatch();
			}
		}
	}
}

void AStandardCombatGameState::ChangeScoreForTeam(EGameTeams Team, int32 DeltaScore)
{
	if(HasAuthority())
	{
		SetScoreForTeam(Team, GetScoreForTeam(Team) + DeltaScore);
	}
}

int32 AStandardCombatGameState::GetScoreForTeam(EGameTeams Team)
{
	switch (Team) { case EGameTeams::None: return 0;
	case EGameTeams::TeamA: return TeamAScore;
	case EGameTeams::TeamB: return TeamBScore;
	default: return 0;
	}
}

void AStandardCombatGameState::CheckForAlivePlayers()
{
	if(HasAuthority())
	{
		bool bAttackersDead = true;
		bool bDefendersDead = true;
		for(auto& PlayerState : GetAllPlayersOfTeam(GetTeamBySide(EBombTeam::Attacker)))
		{
			if(ADefaultPlayerCharacter* AliveCharacter = Cast<ADefaultPlayerCharacter>(PlayerState->GetPawn()))
			{
				if(!AliveCharacter->PlayerHealthComponent->bIsDead)
				{
					bAttackersDead = false;
					break;
				}
			}
		}
		for(auto& PlayerState : GetAllPlayersOfTeam(GetTeamBySide(EBombTeam::Defender)))
		{
			if(ADefaultPlayerCharacter* AliveCharacter = Cast<ADefaultPlayerCharacter>(PlayerState->GetPawn()))
			{
				if(!AliveCharacter->PlayerHealthComponent->bIsDead)
				{
					bDefendersDead = false;
					break;
				}
			}
		}
		if(bAttackersDead || bDefendersDead)
		{
			RoundEndResult = bAttackersDead ? ERoundEndResult::AttackersKilled : ERoundEndResult::DefendersKilled;
			if(!PlantedBomb)
			{
				SelectNewPhase(EGamePhase::EndPhase);
			}
		}
	}

}

EGameTeams AStandardCombatGameState::GetTeamBySide(EBombTeam Side)
{
	EGameTeams AtkTeam = EGameTeams::None;
	EGameTeams DefTeam = EGameTeams::None;
	if(GetSideByTeam(EGameTeams::TeamA) != EBombTeam::None && GetSideByTeam(EGameTeams::TeamB) != EBombTeam::None)
	{
		AtkTeam = GetSideByTeam(EGameTeams::TeamA) == EBombTeam::Attacker ? EGameTeams::TeamA : EGameTeams::TeamB;
		DefTeam = GetSideByTeam(EGameTeams::TeamB) == EBombTeam::Attacker ? EGameTeams::TeamA : EGameTeams::TeamB;
	}
	
	switch (Side) {
	case EBombTeam::None: return EGameTeams::None;
	case EBombTeam::Attacker: return AtkTeam;
	case EBombTeam::Defender: return DefTeam;
	default: return EGameTeams::None;
	}
}

EBombTeam AStandardCombatGameState::GetSideByTeam(EGameTeams Team)
{
	switch (Team) {
	case EGameTeams::None: return EBombTeam::None;
	case EGameTeams::TeamA: return TeamASide;
	case EGameTeams::TeamB: return TeamBSide;
	default: return EBombTeam::None;
	}
}

void AStandardCombatGameState::SetSideOfTeam(EGameTeams Team, EBombTeam Side)
{
	switch (Team) {
	case EGameTeams::None: break;
	case EGameTeams::TeamA: TeamASide = Side;
	case EGameTeams::TeamB: TeamBSide = Side;
	default: ;
	}
}

