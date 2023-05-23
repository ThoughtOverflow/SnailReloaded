// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "Framework/DefaultPlayerState.h"
#include "CombatPlayerState.generated.h"

/**
 * 
 */


UENUM()
enum class EPlayerColor : uint8
{
	None = 0,
	Red = 1,
	Green = 2,
	Blue = 3,
	Magenta = 4,
	Cyan = 5,
	Yellow = 6,
	
};

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
	UPROPERTY(ReplicatedUsing=OnRep_ScoreUpdate)
	int32 PlayerScore;
	UPROPERTY(Replicated=OnRep_ScoreUpdate)
	int32 PlayerPlantCount;
	UPROPERTY(Replicated=OnRep_ScoreUpdate)
	int32 PlayerDefuseCount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing=OnRep_GameTeam)
	EGameTeams CurrentTeam;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_DiedPreviousRound)
	bool IsDeadPreviousRound;
	

	UFUNCTION()
	void OnRep_GameTeam();
	UFUNCTION()
	void OnRep_DiedPreviousRound();
	
	UFUNCTION()
	void OnRep_PlayerMoney();
	UFUNCTION()
	void OnRep_ScoreUpdate();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	//Player colors:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_PlayerColor)
	EPlayerColor PlayerColor;

	UFUNCTION()
	void OnRep_PlayerColor();

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
	UFUNCTION(BlueprintCallable)
	int32 AddPlant();
	UFUNCTION(BlueprintCallable)
	int32 AddDefuse();
	UFUNCTION(BlueprintPure)
	int32 GetKills();
	UFUNCTION(BlueprintPure)
	int32 GetDeaths();
	UFUNCTION(BlueprintPure)
	int32 GetAssists();
	UFUNCTION(BlueprintPure)
	int32 GetPlants();
	UFUNCTION(BlueprintPure)
	int32 GetDefuses();
	UFUNCTION(BlueprintPure)
	int32 GetScores();
	UFUNCTION(BlueprintCallable)
	void CalculateScore();
	UFUNCTION(BlueprintCallable)
	void YouDied();
	UFUNCTION(BlueprintCallable)
	void ResetDeathFlag();
	UFUNCTION(BlueprintPure)
	bool GetDeathState();
	UFUNCTION(BlueprintPure)
	EGameTeams GetTeam();
	UFUNCTION(BlueprintCallable)
	void SetTeam(EGameTeams NewTeam);
	UFUNCTION(BlueprintCallable)
	void SetPlayerColor(EPlayerColor Color);
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerColor(EPlayerColor Color);
	UFUNCTION(BlueprintPure)
	EPlayerColor GetPlayerColor();

	UPROPERTY(EditDefaultsOnly)
	TMap<EPlayerColor, FLinearColor> ColorMap;

	UFUNCTION(BlueprintPure)
	FLinearColor GetColorByEnum(EPlayerColor Color);
	
};
