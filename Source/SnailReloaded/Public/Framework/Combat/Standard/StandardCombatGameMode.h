// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Combat/CombatGameMode.h"
#include "StandardCombatGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API AStandardCombatGameMode : public ACombatGameMode
{
	GENERATED_BODY()

public:

	AStandardCombatGameMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bomb settings")
	TSubclassOf<AActor> BombActor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bomb settings")
	float PlantTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bomb settings")
	float DefuseTime;

	UFUNCTION(BlueprintCallable)
	void BeginPlanting(ADefaultPlayerCharacter* PlantingPlayer);
	UFUNCTION(BlueprintCallable)
	void EndPlanting(ADefaultPlayerCharacter* PlantingPlayer);
	UFUNCTION(BlueprintCallable)
	void BeginDefuse(ADefaultPlayerCharacter* PlantingPlayer);
	UFUNCTION(BlueprintCallable)
	void EndDefuse(ADefaultPlayerCharacter* PlantingPlayer);
	UFUNCTION(BlueprintCallable)
	void PlantBomb(ADefaultPlayerCharacter* Planter);
	UFUNCTION(BlueprintCallable)
	void DefuseBomb(ADefaultPlayerCharacter* Defuser);
	UFUNCTION(BlueprintCallable)
	bool SwapTeamForPlayer(ACombatPlayerState* PlayerState);
	UFUNCTION(BlueprintCallable)
	bool SwapSides();
	
	virtual void EndMatch() override;

protected:

	virtual void InitializeCurrentGame() override;
	virtual void StartRound() override;
	virtual void ProcessPlayerDeath(ACombatPlayerState* PlayerState) override;
	
};
