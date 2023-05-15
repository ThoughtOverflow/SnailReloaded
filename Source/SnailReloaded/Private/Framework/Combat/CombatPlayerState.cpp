// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/CombatPlayerState.h"

#include "Framework/Combat/CombatGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACombatPlayerState::ACombatPlayerState()
{
	CurrentTeam = EGameTeams::None;
}

void ACombatPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if(ACombatGameState* CombatGameState = Cast<ACombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		SetPlayerMoney(CombatGameState->InitialPlayerMoney);
	}
}

void ACombatPlayerState::OnRep_GameTeam()
{
	//If team changes, notify everything through the GameState.
	if(ACombatGameState* CombatGameState = Cast<ACombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		CombatGameState->OnRep_GamePlayers();
	}
}

void ACombatPlayerState::OnRep_PlayerMoney()
{
	
}

void ACombatPlayerState::OnRep_ScoreUpdate()
{
	
}


void ACombatPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACombatPlayerState, PlayerMoney);
	DOREPLIFETIME(ACombatPlayerState, CurrentTeam);
	DOREPLIFETIME(ACombatPlayerState, PlayerAssistCount);
	DOREPLIFETIME(ACombatPlayerState, PlayerDeathCount);
	DOREPLIFETIME(ACombatPlayerState, PlayerKillCount);
	DOREPLIFETIME(ACombatPlayerState, PlayerScore);
}

void ACombatPlayerState::SetPlayerMoney(int32 NewMoney)
{
	if(HasAuthority())
	{
		PlayerMoney = NewMoney;
		OnRep_PlayerMoney();
	}
}

void ACombatPlayerState::ChangePlayerMoney(int32 DeltaMoney)
{
	if(HasAuthority())
	{
		SetPlayerMoney(GetPlayerMoney() + DeltaMoney);
	}
}

int32 ACombatPlayerState::GetPlayerMoney()
{
	return PlayerMoney;
}

int32 ACombatPlayerState::AddKill()
{
	if(HasAuthority())
	{
		PlayerKillCount++;
		OnRep_ScoreUpdate();
	}
	
	return PlayerKillCount;
		
}

int32 ACombatPlayerState::AddDeath()
{
	if(HasAuthority())
	{
		PlayerDeathCount++;
		OnRep_ScoreUpdate();
	}
	return PlayerDeathCount;
}

int32 ACombatPlayerState::AddAssist()
{
	if(HasAuthority())
	{
		PlayerAssistCount++;
		OnRep_ScoreUpdate();
	}
	return PlayerAssistCount;
}

int32 ACombatPlayerState::GetKills()
{
	return PlayerKillCount;
}

int32 ACombatPlayerState::GetDeaths()
{
	return PlayerDeathCount;
}

int32 ACombatPlayerState::GetAssists()
{
	return PlayerAssistCount;
}

EGameTeams ACombatPlayerState::GetTeam()
{
	return CurrentTeam;
}

void ACombatPlayerState::SetTeam(EGameTeams NewTeam)
{
	if(HasAuthority())
	{
		CurrentTeam = NewTeam;
		OnRep_GameTeam();
	}
}
