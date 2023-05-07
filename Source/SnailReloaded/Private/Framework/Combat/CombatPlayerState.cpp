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

void ACombatPlayerState::OnRep_PlayerMoney()
{
	
}

void ACombatPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACombatPlayerState, PlayerMoney);
	DOREPLIFETIME(ACombatPlayerState, CurrentTeam);
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

EGameTeams ACombatPlayerState::GetTeam()
{
	return CurrentTeam;
}

void ACombatPlayerState::SetTeam(EGameTeams NewTeam)
{
	if(HasAuthority())
	{
		CurrentTeam = NewTeam;
	}
}
