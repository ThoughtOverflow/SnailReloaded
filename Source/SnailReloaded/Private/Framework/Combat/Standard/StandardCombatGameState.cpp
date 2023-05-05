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
	
}

void AStandardCombatGameState::OnPhaseExpired(EGamePhase ExpiredPhase)
{
	Super::OnPhaseExpired(ExpiredPhase);

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
		//Do team scoring - round finished.
		
		if(PlantedBomb)
		{
			ExplodeBomb();
		}
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

	RespawnPlayers();
	
	//Give bomb to random player;
	if(HasAuthority())
	{
		int32 randPlayerIndex = FMath::RandRange(0, PlayerArray.Num() - 1);
		if(TObjectPtr<APlayerState> PlayerState = PlayerArray[randPlayerIndex])
		{
			if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(PlayerState->GetPawn()))
			{
				PlayerCharacter->SetHasBomb(true);
			}
		}
	}
	
}

void AStandardCombatGameState::ExplodeBomb()
{
	if(PlantedBomb)
	{
		PlantedBomb->ExplodeBomb();
		PlantedBomb->Destroy();
		PlantedBomb = nullptr;
	}
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
	if(AStandardCombatGameMode* CombatGameMode = Cast<AStandardCombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		CombatGameMode->DefuseBomb(LatestBombInteractor);
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
					TArray<ATeamPlayerStart*> TeamSpecStart = GetPlayerStartsByTeam(CombatPlayerState->GetTeam());

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
					PlayerController->SetControlRotation(RandStart->GetActorRotation());
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
	if(HasAuthority() && bIsPlayerPlanting != bDefusing)
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

void AStandardCombatGameState::OnBombPlanted()
{
	if(HasAuthority())
	{
		SetPlayerDefusing(LatestBombInteractor, false);
		SetPlayerPlanting(LatestBombInteractor, false);
		SelectNewPhase(EGamePhase::PostPlant);
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
