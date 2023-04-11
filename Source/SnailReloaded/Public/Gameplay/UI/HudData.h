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
	UPROPERTY(BlueprintReadWrite)
	ACombatPlayerController* WidgetOwner;

public:

	UHudData* SetPlayerHealthPercentage(float percent);
	UHudData* SetCurrentWeaponName(const FText& NewName);
	UHudData* SetCurrentClipAmmo(int32 NewAmmo);
	UHudData* SetCurrentTotalAmmo(int32 NewAmmo);
	void Submit();
	
	float GetPlayerHealthPercentage() const;
	const FText& GetCurrentWeaponName() const;
	int32 GetCurrentClipAmmo() const;
	int32 GetCurrentTotalAmmo() const;
	

	
	
};
