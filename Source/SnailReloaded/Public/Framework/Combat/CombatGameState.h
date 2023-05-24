// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatGameMode.h"
#include "CombatPlayerState.h"
#include "Framework/DefaultGameState.h"
#include "World/Objects/MinimapDefinition.h"
#include "World/Objects/OverviewCamera.h"
#include "CombatGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDied);

UENUM(BlueprintType)
enum class EBombTeam : uint8
{
	None = 0,
	Attacker = 1,
	Defender = 2,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGamePlayersUpdated);

class ATeamPlayerStart;
struct FGamePhase;
UCLASS()
class SNAILRELOADED_API ACombatGameState : public ADefaultGameState
{
public:


private:
	GENERATED_BODY()

public:

	ACombatGameState();


	
	UPROPERTY(BlueprintReadWrite)
	int32 InitialPlayerMoney;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADefaultPlayerCharacter> PlayerCharacterClass;

	UPROPERTY(BlueprintAssignable)
	FOnGamePlayersUpdated OnGamePlayersUpdated;

	UPROPERTY(BlueprintReadWrite)
	int32 BaseScore;
	UPROPERTY(BlueprintReadWrite)
	int32 KillValue;
	UPROPERTY(BlueprintReadWrite)
	int32 DeathValue;
	UPROPERTY(BlueprintReadWrite)
	int32 AssistValue;
	UPROPERTY(BlueprintReadWrite)
	int32 BombValue;
	UPROPERTY(BlueprintReadWrite)
	int32 KillReward;
	UPROPERTY(BlueprintReadWrite)
	int32 PlantReward;
	UPROPERTY(BlueprintReadWrite)
	int32 VictorReward;
	UPROPERTY(BlueprintReadWrite)
	int32 LoserReward;
	UPROPERTY(BlueprintReadWrite)
	int32 LoserDeadReward;

	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDied OnPlayerDied;


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_MatchPause)
	bool bMatchPaused;
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
	UPROPERTY()
	AMinimapDefinition* MinimapDefinition;
	
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

	UFUNCTION()
	void OnRep_MatchPause();

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
	virtual TArray<ATeamPlayerStart*> GetPlayerStartsByTeam(EBombTeam Team);
	UFUNCTION(BlueprintCallable)
	virtual TArray<ATeamPlayerStart*> GetAllPlayerStarts();
	UFUNCTION(BlueprintPure)
	AOverviewCamera* GetLevelOverviewCamera();
	UFUNCTION(BlueprintCallable)
	TArray<ACombatPlayerState*> GetAllPlayersOfTeam(EGameTeams Team);

	UFUNCTION()
	void OnRep_GamePlayers();

	UFUNCTION(BlueprintCallable)
	void PauseMatch();
	UFUNCTION(BlueprintCallable)
	void UnpauseMatch();

	UFUNCTION(BlueprintPure)
	AMinimapDefinition* GetMinimapDefinition();

	UFUNCTION(BlueprintCallable)
	int32 GetBaseScore() const;
	UFUNCTION(BlueprintCallable)
	int32 GetKillValue() const;
	UFUNCTION(BlueprintCallable)
	int32 GetDeathValue() const;
	UFUNCTION(BlueprintCallable)
	int32 GetAssistValue() const;
	UFUNCTION(BlueprintCallable)
	int32 GetBombValue() const;
	UFUNCTION(BlueprintCallable)
	int32 GetKillReward() const;
	UFUNCTION(BlueprintCallable)
	int32 GetPlantReward() const;
	UFUNCTION(BlueprintCallable)
	int32 GetVictorReward() const;
	UFUNCTION(BlueprintCallable)
	int32 GetLoserReward() const;
	UFUNCTION(BlueprintCallable)
	int32 GetLoserDeadReward() const;

	UFUNCTION(BlueprintCallable)
	void NotifyPlayerDeath(ACombatPlayerState* Player);


	
	
};

