// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HudData.generated.h"

class ACombatPlayerController;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SNAILRELOADED_API UHudData : public UObject
{
	
GENERATED_BODY()

public:

	UHudData();

	static UHudData* CreatePlayerHudWrapper(ACombatPlayerController* PlayerController);

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerHealthPercentage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CurrentWeaponName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentClipAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentTotalAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerShieldHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsReloading;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowPlantHint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowDefuseHint;
	UPROPERTY(BlueprintReadWrite)
	ACombatPlayerController* WidgetOwner;

public:

	UHudData* SetPlayerHealthPercentage(float percent);
	UHudData* SetCurrentWeaponName(const FText& NewName);
	UHudData* SetCurrentClipAmmo(int32 NewAmmo);
	UHudData* SetCurrentTotalAmmo(int32 NewAmmo);
	UHudData* SetPlayerShieldHealth(float NewHealth);
	UHudData* SetReloading(bool bReloading);
	UHudData* SetShowPlantHint(bool bShow);
	UHudData* SetShowDefuseHint(bool bShow);
	void Submit();
	
	float GetPlayerHealthPercentage() const;
	const FText& GetCurrentWeaponName() const;
	int32 GetCurrentClipAmmo() const;
	int32 GetCurrentTotalAmmo() const;
	float GetPlayerShieldHealth() const;
	bool GetIsReloading() const;
	bool GetShowPlantHint();
	bool GetShowDefuseHint();
	

	
	
};
