// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatGameMode.h"
#include "Framework/DefaultGameState.h"
#include "CombatGameState.generated.h"

/**
 * 
 */


struct FGamePhase;
UCLASS()
class SNAILRELOADED_API ACombatGameState : public ADefaultGameState
{
	GENERATED_BODY()

public:

	ACombatGameState();

	UPROPERTY(BlueprintReadWrite)
	int32 InitialPlayerMoney;

protected:

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_GamePhase)
	FGamePhase CurrentGamePhase;
	UFUNCTION()
	void OnRep_GamePhase();
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_RoundCounter)
	int32 CurrentRound;
	UFUNCTION()
	void OnRep_RoundCounter();

	FTimerHandle PhaseTimer;

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
	
	
};
