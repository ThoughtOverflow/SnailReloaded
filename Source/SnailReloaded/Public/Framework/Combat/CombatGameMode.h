// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArmoredHealthComponent.h"
#include "Framework/DefaultGameMode.h"
#include "CombatGameMode.generated.h"

class AWeaponBase;
class ADefaultPlayerCharacter;
struct FDamageRequest;
struct FDamageResponse;
/**
 * 
 */

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	None = 0,
	Preparation = 1,
	ActiveGame = 2,
	PostPlant = 3,
	EndPhase = 4
};

USTRUCT(BlueprintType)
struct FGamePhase
{
	GENERATED_BODY()

public:

	FGamePhase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EGamePhase GamePhase;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float PhaseTimeSeconds;
	
	
};

UCLASS()
class SNAILRELOADED_API ACombatGameMode : public ADefaultGameMode
{
	GENERATED_BODY()

public:

	ACombatGameMode();

	

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Buy system")
	TMap<EItemIdentifier, int32> ItemPriceList;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Buy system")
	TMap<EItemIdentifier, TSubclassOf<AWeaponBase>> WeaponIdTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Buy system")
	TArray<FShieldProperties> ShieldDataTable;

	FShieldProperties* FindShieldDataByType(EItemIdentifier ShieldIdentifier);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game specific properties")
	TArray<EGamePhase> AllowedPhases;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game specific properties")
	TArray<FGamePhase> GamePhases;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game specific properties")
	int32 PlayerStartMoney;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game specific properties")
	int32 MaxRounds;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game specific properties")
	bool bAllowOvertime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game specific properties", meta = (EditCondition = "bAllowOvertime"))
	int32 MaxOvertimeRounds;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game specific properties", meta = (EditCondition = "bAllowOvertime"))
	int32 OvertimeScoreDifference;

	
public:


	UFUNCTION(BlueprintCallable)
	FDamageResponse ChangeObjectHealth(FDamageRequest& DamageRequest);

	UFUNCTION(BlueprintCallable)
	bool PurchaseItem(ADefaultPlayerCharacter* PlayerCharacter, EItemIdentifier ItemIdentifier);
	UFUNCTION(BlueprintCallable)
	bool SellItem(ADefaultPlayerCharacter* PlayerCharacter, EItemIdentifier ItemIdentifier);

	UFUNCTION(BlueprintCallable)
	virtual void InitializeCurrentGame();
	UFUNCTION(BlueprintCallable)
	bool GetGamePhaseByType(EGamePhase Phase, FGamePhase& RefGamePhase);
	UFUNCTION(BlueprintCallable)
	void StartRound();
	UFUNCTION(BlueprintCallable)
	virtual void ProcessPlayerDeath(ADefaultPlayerCharacter* PlayerCharacter);
	
};
