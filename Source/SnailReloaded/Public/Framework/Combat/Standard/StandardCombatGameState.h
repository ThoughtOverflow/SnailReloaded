// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Combat/CombatGameState.h"
#include "StandardCombatGameState.generated.h"


UENUM(BlueprintType)
enum class ERoundEndResult : uint8
{
	None = 0,
	BombExplode = 1,
	BombDefuse = 2,
	AttackersKilled = 3,
	DefendersKilled = 4,
	OutOfTime = 6,
};

class ABomb;
/**
 * 
 */
UCLASS()
class SNAILRELOADED_API AStandardCombatGameState : public ACombatGameState
{
	GENERATED_BODY()

public:

	AStandardCombatGameState();

	UPROPERTY(BlueprintReadOnly, Replicated)
	float PlantTime;
	UPROPERTY(BlueprintReadOnly, Replicated)
	float DefuseTime;
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 NumberOfRounds;

	UPROPERTY(BlueprintReadOnly, Replicated)
	ABomb* PlantedBomb;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AActor> BarrierClass;
	UPROPERTY(ReplicatedUsing = OnRep_Barriers)
	bool bBarriersActive;

	UFUNCTION()
	void OnRep_Barriers();
	
protected:

	virtual void OnPhaseExpired(EGamePhase ExpiredPhase) override;
	virtual void OnPhaseSelected(EGamePhase NewPhase) override;
	virtual void StartNewRound() override;

	UFUNCTION()
	void ToggleBarriers(bool bShow);

	UFUNCTION()
	void ExplodeBomb();

	UPROPERTY(ReplicatedUsing = OnRep_PlantDefuse)
	bool bIsPlayerPlanting;
	UPROPERTY(ReplicatedUsing = OnRep_PlantDefuse)
	bool bIsPlayerDefusing;
	UPROPERTY(Replicated)
	ADefaultPlayerCharacter* LatestBombInteractor;

	UFUNCTION()
	void OnRep_PlantDefuse();

	FTimerHandle PlantTimer;
	FTimerHandle DefuseTimer;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 TeamAScore;
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 TeamBScore;
	
	UFUNCTION()
	void PlantTimerCallback();
	UFUNCTION()
	void DefuseTimerCallback();

	UFUNCTION()
	void RespawnPlayers();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > &OutLifetimeProps) const override;

	UPROPERTY()
	ERoundEndResult RoundEndResult;

	UFUNCTION()
	void HandleTeamScoring();

	UPROPERTY(BlueprintReadWrite, Replicated)
	EBombTeam TeamASide;
	UPROPERTY(BlueprintReadWrite, Replicated)
	EBombTeam TeamBSide;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeaponBase> DefaultMelee;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeaponBase> DefaultPrimary;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeaponBase> DefaultSecondary;
	
	UPROPERTY()
	bool bAfterSideSwap;
	
public:

	UFUNCTION(BlueprintCallable)
	void SetPlayerPlanting(ADefaultPlayerCharacter* Player, bool bPlanting);
	UFUNCTION(BlueprintCallable)
	void SetPlayerDefusing(ADefaultPlayerCharacter* Player, bool bDefusing);
	UFUNCTION(BlueprintPure)
	bool IsSomeonePlanting();
	UFUNCTION(BlueprintPure)
	bool IsSomeoneDefusing();
	UFUNCTION(BlueprintCallable)
	bool IsPlayerPlanting(ADefaultPlayerCharacter* Player);
	UFUNCTION(BlueprintCallable)
	bool IsPlayerDefusing(ADefaultPlayerCharacter* Player);
	UFUNCTION()
	void OnBombPlanted();
	UFUNCTION()
	void OnBombDefused();
	UFUNCTION(BlueprintPure)
	float GetBombActionTimeRemaining();
	UFUNCTION(BlueprintCallable)
	EGameTeams GetWinningTeam();
	UFUNCTION(BlueprintCallable)
	EGameTeams GetLosingTeam();
	UFUNCTION(BlueprintCallable)
	void SetScoreForTeam(EGameTeams Team, int32 NewScore);
	UFUNCTION(BlueprintCallable)
	void ChangeScoreForTeam(EGameTeams Team, int32 DeltaScore);
	UFUNCTION(BlueprintPure)
	int32 GetScoreForTeam(EGameTeams Team);
	UFUNCTION(BlueprintCallable)
	void NewRoundPayout();
	UFUNCTION()
	void CheckForAlivePlayers();
	UFUNCTION(BlueprintCallable)
	EGameTeams GetTeamBySide(EBombTeam Side);
	UFUNCTION(BlueprintCallable)
	EBombTeam GetSideByTeam(EGameTeams Team);
	UFUNCTION(BlueprintCallable)
	void SetSideOfTeam(EGameTeams Team, EBombTeam Side);

	UFUNCTION()
	void NotifySwapSides();
	
};
