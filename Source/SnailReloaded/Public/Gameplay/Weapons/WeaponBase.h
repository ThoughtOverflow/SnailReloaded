// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{

	Primary = 2,
	Secondary = 1,
	Melee = 0
	
};

UENUM(BlueprintType)
enum class EWeaponMode : uint8
{
	Automatic = 0,
	SemiAutomatic = 1,
	Burst = 2
};

UCLASS()
class SNAILRELOADED_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (ClampMin=0))
	float MinimumFireDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated)
	EWeaponMode WeaponMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated)
	EWeaponSlot WeaponSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", ReplicatedUsing = OnRep_ClipAmmo, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	int32 CurrentClipAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", ReplicatedUsing = OnRep_TotalAmmo, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	int32 CurrentTotalAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	int32 MaxClipAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	int32 MaxTotalAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	float ReloadTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	bool bIsReloading;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = "WeaponMode == EWeaponMode::Burst && WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	int32 BurstAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = "WeaponMode != EWeaponMode::SemiAutomatic && WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	float FireRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated)
	bool bUseConstantDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = "!bUseConstantDamage", EditConditionHides=true))
	UCurveFloat* DamageCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = "bUseConstantDamage", EditConditionHides=true))
	float ConstantDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	float ProjectilePenetrationMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	bool bShotgunSpread;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings|Shotugun Settings", Replicated, meta = (EditCondition = "bShotgunSpread && WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	int32 NumOfPellets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
	UNiagaraSystem* ImpactParticleSystem;
	
	/**
	 * @brief Maximum projectile deviation from barrel in degrees.
	 * @details Sets the maximum projectile deviation from the barrel in degrees. Setting it to 0 will cause the weapon to fire all pellets in a straight line. Max angle is 180 degrees.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings|Shotugun Settings", Replicated,
		meta = (EditCondition = "bShotgunSpread && WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true,
			ClampMin = 0, ClampMax = 180))
	float BarrelMaxDeviation;
	/**
	 * @brief Minimum projectile deviation from barrel in degrees.
	 * @details Sets the Minimum projectile deviation from the barrel in degrees. Max angle is 180 degrees.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings|Shotugun Settings", Replicated,
		meta = (EditCondition = "bShotgunSpread && WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true,
			ClampMin = 0, ClampMax = 180))
	float BarrelMinDeviation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
	bool bIsEquipped;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnRep_TotalAmmo();
	UFUNCTION()
	void OnRep_ClipAmmo();
	UFUNCTION()
	void OnRep_Equipped();

};
