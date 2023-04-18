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
	TMap<EItemIdentifier, FShieldProperties> ShieldDataTable;
	
public:


	UFUNCTION(BlueprintCallable)
	FDamageResponse ChangeObjectHealth(FDamageRequest DamageRequest);

	UFUNCTION(BlueprintCallable)
	bool PurchaseItem(ADefaultPlayerCharacter* PlayerCharacter, EItemIdentifier ItemIdentifier);
	UFUNCTION(BlueprintCallable)
	bool SellItem(ADefaultPlayerCharacter* PlayerCharacter, EItemIdentifier ItemIdentifier);
	
};
