// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/CombatGameState.h"

#include "Framework/Combat/CombatPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "World/Objects/OverviewCamera.h"
#include "World/Objects/TeamPlayerStart.h"


ACombatGameState::ACombatGameState()
{
	InitialPlayerMoney = 5000;
	CurrentRound = 0;
	BaseScore =25;
	KillValue =30;
	DeathValue=5;
	BombValue =10;
	KillReward =200;
	PlantReward =300;
	VictorReward =3000;
	LoserReward =2300;
	SurviveReward = 1000;
	MoneyCap = 9000;
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
	if(AMinimapDefinition* MapDef = Cast<AMinimapDefinition>(UGameplayStatics::GetActorOfClass(GetWorld(), AMinimapDefinition::StaticClass())))
	{
		MinimapDefinition = MapDef;
	}
	checkf(MinimapDefinition, TEXT("No minimap definition found in level!"));
}

void ACombatGameState::OnRep_MatchPause()
{
	if(bMatchPaused)
	{
		GetWorldTimerManager().PauseTimer(PhaseTimer);
	}else
	{
		GetWorldTimerManager().UnPauseTimer(PhaseTimer);
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
	if(NewPhase == EGamePhase::PostPlant)
	{
		for(auto& PlayerState : GetAllGamePlayers())
		{
			//Add prep phase noti:
			if(ACombatPlayerController* CombatPlayerController = Cast<ACombatPlayerController>(PlayerState->GetPlayerController()))
			{
				CombatPlayerController->TriggerGameNotification(ENotificationType::PrepPhase);
			}
		}
	}
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
	//Check if it has to be paused;
	OnRep_MatchPause();
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
	if(!GetWorld()->bIsTearingDown)
	{
		OnGamePlayersUpdated.Broadcast();	
	}
}

void ACombatGameState::PauseMatch()
{
	if(HasAuthority())
	{
		bMatchPaused = true;
		OnRep_MatchPause();
	}
}

void ACombatGameState::UnpauseMatch()
{
	if(HasAuthority())
	{
		bMatchPaused = true;
		OnRep_MatchPause();
	}
}

AMinimapDefinition* ACombatGameState::GetMinimapDefinition()
{
	return MinimapDefinition;
}



void ACombatGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACombatGameState, CurrentGamePhase);
	DOREPLIFETIME(ACombatGameState, CurrentRound);
	DOREPLIFETIME(ACombatGameState, GamePlayers);
	DOREPLIFETIME(ACombatGameState, bMatchPaused);
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
		for(ACombatPlayerState* PlayerState:GetAllGamePlayers())
		{
			if(CurrentRound == 1)
			{
				PlayerState->SetPlayerMoney(InitialPlayerMoney);
			}
			//Reset HP and Shield.
			if(ADefaultPlayerCharacter* DefaultPlayerCharacter = Cast<ADefaultPlayerCharacter>(PlayerState->GetPawn()))
			{
				DefaultPlayerCharacter->PlayerHealthComponent->ResetHealth();
				DefaultPlayerCharacter->PlayerHealthComponent->ResetShieldHeath();
			}
			//Add prep phase noti:
			if(ACombatPlayerController* CombatPlayerController = Cast<ACombatPlayerController>(PlayerState->GetPlayerController()))
			{
				CombatPlayerController->TriggerGameNotification(ENotificationType::PrepPhase);
			}
			
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

TArray<ATeamPlayerStart*> ACombatGameState::GetPlayerStartsByTeam(EBombTeam Team)
{
	TArray<ATeamPlayerStart*> ReturnSpawns;
	if(HasAuthority())
	{
		if(Team == EBombTeam::None)
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

TArray<ACombatPlayerState*> ACombatGameState::GetAllPlayersOfTeam(EGameTeams Team)
{
	TArray<ACombatPlayerState*> Players;
	for(auto& PlayerState : PlayerArray)
	{
		if(ACombatPlayerState* CombatPlayerState = Cast<ACombatPlayerState>(PlayerState))
		{
			if(CombatPlayerState->GetTeam() == Team)
			{
				Players.Add(CombatPlayerState);
			}
		}
	}
	return Players;
}

int32 ACombatGameState::GetBaseScore() const
{
	return BaseScore;
}

int32 ACombatGameState::GetKillValue() const
{
	return KillValue;
}

int32 ACombatGameState::GetDeathValue() const
{
	return DeathValue;
}

int32 ACombatGameState::GetAssistValue() const
{
	return AssistValue;
}

int32 ACombatGameState::GetBombValue() const
{
	return BombValue;
}

int32 ACombatGameState::GetKillReward() const
{
	return KillReward;
}

int32 ACombatGameState::GetPlantReward() const
{
	return PlantReward;
}

int32 ACombatGameState::GetVictorReward() const
{
	return VictorReward;
}
int32 ACombatGameState::GetLoserReward() const
{
	return LoserReward;
}

int32 ACombatGameState::GetSurviveReward() const
{
	return SurviveReward;
}

void ACombatGameState::NotifyPlayerDeath(ACombatPlayerState* Player)
{
	OnPlayerDied.Broadcast();
}

void ACombatGameState::PlayAnnouncement(EAnnouncement Announcement)
{
	for(auto& PlayerState : GetAllGamePlayers())
	{
		if(ACombatPlayerController* CombatPlayerController = Cast<ACombatPlayerController>(PlayerState->GetOwningController()))
		{
			if(Announcements.Contains(Announcement))
			{
				CombatPlayerController->Client_PlayAnnouncement(*Announcements.Find(Announcement));
			}
			
			
		}
	}
}
