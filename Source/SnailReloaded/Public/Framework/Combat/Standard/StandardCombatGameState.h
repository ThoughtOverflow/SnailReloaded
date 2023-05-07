// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Combat/CombatGameState.h"
#include "StandardCombatGameState.generated.h"

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

	UPROPERTY()
	ABomb* PlantedBomb;

protected:

	virtual void OnPhaseExpired(EGamePhase ExpiredPhase) override;
	virtual void OnPhaseSelected(EGamePhase NewPhase) override;
	virtual void StartNewRound() override;

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

	UFUNCTION()
	void PlantTimerCallback();
	UFUNCTION()
	void DefuseTimerCallback();

	UFUNCTION()
	void RespawnPlayers();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > &OutLifetimeProps) const override;
	
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
	UFUNCTION(BlueprintPure)
	float GetBombActionTimeRemaining();
	
	
};
