// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/CombatGameState.h"

#include "Framework/Combat/CombatPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "World/Objects/OverviewCamera.h"
#include "World/Objects/TeamPlayerStart.h"

ACombatGameState::ACombatGameState()
{
	InitialPlayerMoney = 5000;
	CurrentRound = 0;
}

void ACombatGameState::OnRep_GamePhase()
{
	//Set timer:
	SetPhaseTimer();
	//Notify:
	OnPhaseSelected(CurrentGamePhase.GamePhase);
}

void ACombatGameState::BeginPlay()
{
	Super::BeginPlay();

	if(AOverviewCamera* OverviewCamera = Cast<AOverviewCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), AOverviewCamera::StaticClass())))
	{
		LevelOverviewCamera = OverviewCamera;
	}
	
}

void ACombatGameState::OnRep_RoundCounter()
{
	
}

void ACombatGameState::PhaseTimerCallback()
{
	//Call phase expired notify:
	OnPhaseExpired(CurrentGamePhase.GamePhase);
}

void ACombatGameState::OnPhaseExpired(EGamePhase ExpiredPhase)
{
	//Phase expired - allow override.
	
	//Close all buy menus.
	if(ExpiredPhase == EGamePhase::Preparation)
	{
		if(HasAuthority())
		{
			for(TObjectPtr<APlayerState> PlayerState : PlayerArray)
			{
				if(ACombatPlayerController* PlayerController = Cast<ACombatPlayerController>(PlayerState->GetPlayerController()))
				{
					PlayerController->ToggleBuyMenu(false);
				}
			}
		}
	}
}

void ACombatGameState:: OnPhaseSelected(EGamePhase NewPhase)
{
	
}

void ACombatGameState::SetPhaseTimer()
{
	// if none, return;
	if(CurrentGamePhase.GamePhase == EGamePhase::None) return;
	
	if(GetWorldTimerManager().IsTimerActive(PhaseTimer))
	{
		CancelPhaseTimer();
	}
	GetWorldTimerManager().SetTimer(PhaseTimer, this, &ACombatGameState::PhaseTimerCallback, CurrentGamePhase.PhaseTimeSeconds);
}

void ACombatGameState::CancelPhaseTimer()
{
	if(GetWorldTimerManager().IsTimerActive(PhaseTimer))
	{
		GetWorldTimerManager().ClearTimer(PhaseTimer);
	}
}

void ACombatGameState::SelectNewPhase(EGamePhase NewPhase)
{
	if(HasAuthority())
	{
		if(ACombatGameMode* CombatGameMode = Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			FGamePhase FoundPhase;
			if(CombatGameMode->GetGamePhaseByType(NewPhase, FoundPhase))
			{
				SetCurrentGamePhase(FoundPhase);
			}
		}
	}
}

void ACombatGameState::OnRep_GamePlayers()
{
	OnGamePlayersUpdated.Broadcast();
}

void ACombatGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACombatGameState, CurrentGamePhase);
	DOREPLIFETIME(ACombatGameState, CurrentRound);
	DOREPLIFETIME(ACombatGameState, GamePlayers);
}

void ACombatGameState::CurrentGameInitialized()
{
	//Callback to notify game state at start.
}

FGamePhase ACombatGameState::GetCurrentGamePhase()
{
	return CurrentGamePhase;
}

void ACombatGameState::SetCurrentGamePhase(FGamePhase NewPhase)
{
	if(HasAuthority())
	{
		if(CurrentGamePhase.GamePhase != NewPhase.GamePhase)
		{
			CurrentGamePhase = NewPhase;
			OnRep_GamePhase();
		}
	}
}

float ACombatGameState::GetRemainingPhaseTime()
{
	return GetWorldTimerManager().IsTimerActive(PhaseTimer) ? GetWorldTimerManager().GetTimerRemaining(PhaseTimer) : 0.f;
}

void ACombatGameState::StartNewRound()
{
	//Restart from beginning.
	if(HasAuthority())
	{
		
		if(ACombatGameMode* CombatGameMode = Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			CombatGameMode->StartRound();
		}
	}
}

int32 ACombatGameState::GetCurrentRound()
{
	return CurrentRound;
}

void ACombatGameState::SetCurrentRound(int32 NewRound)
{
	if(HasAuthority())
	{
		CurrentRound = NewRound;
		OnRep_RoundCounter();
	}
}

void ACombatGameState::AddGamePlayer(ACombatPlayerState* PlayerState)
{
	if(HasAuthority())
	{
		if(!GamePlayers.Contains(PlayerState))
		{
			GamePlayers.Add(PlayerState);
			OnRep_GamePlayers();
		}
	}
}

void ACombatGameState::RemoveGamePlayer(ACombatPlayerState* PlayerState)
{
	if(HasAuthority())
	{
		if(GamePlayers.Contains(PlayerState))
		{
			GamePlayers.Remove(PlayerState);
			OnRep_GamePlayers();
		}
	}
}

TArray<ACombatPlayerState*>& ACombatGameState::GetAllGamePlayers()
{
	return GamePlayers;
}

TArray<ATeamPlayerStart*> ACombatGameState::GetPlayerStartsByTeam(EGameTeams Team)
{
	TArray<ATeamPlayerStart*> ReturnSpawns;
	if(HasAuthority())
	{
		if(Team == EGameTeams::None)
		{
			return GetAllPlayerStarts();
		}
		TArray<AActor*> FoundSpawns;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeamPlayerStart::StaticClass(), FoundSpawns);
		for(auto& Spawn : FoundSpawns)
		{
			if(ATeamPlayerStart* PlayerStart = Cast<ATeamPlayerStart>(Spawn))
			{
				if(PlayerStart->AssignedTeam == Team)
				{
					ReturnSpawns.Add(PlayerStart);
				}
			}
		}
	}
	return ReturnSpawns;
}

TArray<ATeamPlayerStart*> ACombatGameState::GetAllPlayerStarts()
{
	TArray<ATeamPlayerStart*> ReturnSpawns;
	if(HasAuthority())
	{
		TArray<AActor*> FoundSpawns;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeamPlayerStart::StaticClass(), FoundSpawns);
		for(auto& Spawn : FoundSpawns)
		{
			if(ATeamPlayerStart* PlayerStart = Cast<ATeamPlayerStart>(Spawn))
			{
				ReturnSpawns.Add(PlayerStart);
			}
		}
	}
	return ReturnSpawns;
}

AOverviewCamera* ACombatGameState::GetLevelOverviewCamera()
{
	return LevelOverviewCamera;
}
