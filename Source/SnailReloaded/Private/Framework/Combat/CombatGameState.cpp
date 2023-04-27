// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/CombatGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

void ACombatGameState::OnRep_RoundCounter()
{
	
}

void ACombatGameState::OnPhaseExpired()
{
	//Phase expired - allow override.
}

void ACombatGameState::OnPhaseSelected(EGamePhase NewPhase)
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
	GetWorldTimerManager().SetTimer(PhaseTimer, this, &ACombatGameState::OnPhaseExpired, CurrentGamePhase.PhaseTimeSeconds);
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

void ACombatGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACombatGameState, CurrentGamePhase);
	DOREPLIFETIME(ACombatGameState, CurrentRound);
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
