// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "Framework/DefaultPlayerState.h"
#include "CombatPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API ACombatPlayerState : public ADefaultPlayerState
{
	GENERATED_BODY()

public:

	ACombatPlayerState();

	virtual void BeginPlay() override;

protected:
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerMoney)
	int32 PlayerMoney;
	UPROPERTY(ReplicatedUsing=OnRep_ScoreUpdate)
	int32 PlayerKillCount;
	UPROPERTY(ReplicatedUsing=OnRep_ScoreUpdate)
	int32 PlayerDeathCount;
	UPROPERTY(ReplicatedUsing=OnRep_ScoreUpdate)
	int32 PlayerAssistCount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing=OnRep_GameTeam)
	EGameTeams CurrentTeam;

	UFUNCTION()
	void OnRep_GameTeam();
	
	UFUNCTION()
	void OnRep_PlayerMoney();
	UFUNCTION()
	void OnRep_ScoreUpdate();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

public:

	UFUNCTION(BlueprintCallable)
	void SetPlayerMoney(int32 NewMoney);
	UFUNCTION(BlueprintCallable)
	void ChangePlayerMoney(int32 DeltaMoney);
	UFUNCTION(BlueprintPure)
	int32 GetPlayerMoney();
	UFUNCTION(BlueprintCallable)
	int32 AddKill();
	UFUNCTION(BlueprintCallable)
	int32 AddDeath();
	UFUNCTION(BlueprintCallable)
	int32 AddAssist();
	UFUNCTION(BlueprintPure)
	EGameTeams GetTeam();
	UFUNCTION(BlueprintCallable)
	void SetTeam(EGameTeams NewTeam);
	
};
