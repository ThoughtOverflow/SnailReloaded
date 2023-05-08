// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatGameMode.h"
#include "CombatPlayerState.h"
#include "Framework/DefaultGameState.h"
#include "World/Objects/OverviewCamera.h"
#include "CombatGameState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGamePlayersUpdated);

class ATeamPlayerStart;
struct FGamePhase;
UCLASS()
class SNAILRELOADED_API ACombatGameState : public ADefaultGameState
{
	GENERATED_BODY()

public:

	ACombatGameState();

	UPROPERTY(BlueprintReadWrite)
	int32 InitialPlayerMoney;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADefaultPlayerCharacter> PlayerCharacterClass;

	UPROPERTY(BlueprintAssignable)
	FOnGamePlayersUpdated OnGamePlayersUpdated;

protected:

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_GamePhase)
	FGamePhase CurrentGamePhase;
	UFUNCTION()
	void OnRep_GamePhase();
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_RoundCounter)
	int32 CurrentRound;
	UFUNCTION()
	void OnRep_RoundCounter();

	UPROPERTY(BlueprintReadWrite)
	AOverviewCamera* LevelOverviewCamera;
	
	FTimerHandle PhaseTimer;

	UPROPERTY(ReplicatedUsing=OnRep_GamePlayers)
	TArray<ACombatPlayerState*> GamePlayers;

	UFUNCTION()
	void PhaseTimerCallback();
	UFUNCTION()
	virtual void OnPhaseExpired(EGamePhase ExpiredPhase);
	UFUNCTION()
	virtual void OnPhaseSelected(EGamePhase NewPhase);
	UFUNCTION()
	virtual void SetPhaseTimer();
	UFUNCTION()
	virtual void CancelPhaseTimer();
	UFUNCTION()
	virtual void SelectNewPhase(EGamePhase NewPhase);

	//override
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > &OutLifetimeProps) const override;

	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable)
	virtual void CurrentGameInitialized();
	UFUNCTION(BlueprintPure)
	FGamePhase GetCurrentGamePhase();
	UFUNCTION(BlueprintCallable)
	void SetCurrentGamePhase(FGamePhase NewPhase);
	UFUNCTION(BlueprintPure)
	float GetRemainingPhaseTime();
	UFUNCTION(BlueprintCallable)
	virtual void StartNewRound();

	UFUNCTION(BlueprintPure)
	int32 GetCurrentRound();
	UFUNCTION(BlueprintCallable)
	void SetCurrentRound(int32 NewRound);

	UFUNCTION(BlueprintCallable)
	void AddGamePlayer(ACombatPlayerState* PlayerState);
	UFUNCTION(BlueprintCallable)
	void RemoveGamePlayer(ACombatPlayerState* PlayerState);
	UFUNCTION(BlueprintPure)
	TArray<ACombatPlayerState*>& GetAllGamePlayers();

	UFUNCTION(BlueprintCallable)
	virtual TArray<ATeamPlayerStart*> GetPlayerStartsByTeam(EGameTeams Team);
	UFUNCTION(BlueprintCallable)
	virtual TArray<ATeamPlayerStart*> GetAllPlayerStarts();
	UFUNCTION(BlueprintPure)
	AOverviewCamera* GetLevelOverviewCamera();

	UFUNCTION()
	void OnRep_GamePlayers();

	
	
};

