// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/CombatGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACombatGameState::ACombatGameState()
{
	InitialPlayerMoney = 5000;
}

void ACombatGameState::OnRep_GamePhase()
{
	//Set timer:
	SetPhaseTimer();
}

void ACombatGameState::OnPhaseExpired()
{
	//Phase expired - allow override.
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
