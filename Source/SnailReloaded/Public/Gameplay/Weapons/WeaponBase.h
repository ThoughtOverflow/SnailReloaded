// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UENUM(BlueprintType)
enum class EItemIdentifier : uint8
{
	None = 0,
	LightShield = 1,
	HeavyShield = 2,
	DefaultMelee = 3,
	DefaultAR = 4,
	DefaultShotgun = 5,
	NullShield = 6
};

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmmoUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReload);

UCLASS()
class SNAILRELOADED_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
	EItemIdentifier ItemIdentifier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
	FText WeaponName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (ClampMin=0))
	float MinimumFireDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	EWeaponMode WeaponMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated)
	EWeaponSlot WeaponSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	float ReloadTime;
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
	
	UPROPERTY(BlueprintAssignable)
	FOnAmmoUpdated OnAmmoUpdated;
	UPROPERTY(BlueprintAssignable)
	FOnReload OnReload;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings|Ammo Settings", ReplicatedUsing = OnRep_ClipAmmo, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	int32 CurrentClipAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings|Ammo Settings", Replicated, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	int32 MaxClipAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings|Ammo Settings", ReplicatedUsing = OnRep_TotalAmmo, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	int32 CurrentTotalAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings|Ammo Settings", Replicated, meta = (EditCondition = "WeaponSlot != EWeaponSlot::Melee", EditConditionHides=true))
	int32 MaxTotalAmmo;
	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
	bool bIsEquipped;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_Reloading)
	bool bIsReloading;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_CanSell)
	bool bCanSell;

public:
	

protected:
	UFUNCTION()
	void OnRep_TotalAmmo();
	UFUNCTION()
	void OnRep_ClipAmmo();
	UFUNCTION()
	void OnRep_Equipped();
	UFUNCTION()
	void OnRep_Reloading();
	UFUNCTION()
	void OnRep_CanSell();

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetCurrentClipAmmo(int32 NewAmmo);
	UFUNCTION(BlueprintCallable)
	void SetCurrentTotalAmmo(int32 NewAmmo);
	UFUNCTION(BlueprintPure)
	int32 GetCurrentClipAmmo();
	UFUNCTION(BlueprintPure)
	int32 GetCurrentTotalAmmo();

	UFUNCTION(BlueprintCallable)
	void SetMaxClipAmmo(int32 NewAmmo);
	UFUNCTION(BlueprintCallable)
	void SetMaxTotalAmmo(int32 NewAmmo);
	UFUNCTION(BlueprintPure)
	int32 GetMaxClipAmmo();
	UFUNCTION(BlueprintPure)
	int32 GetMaxTotalAmmo();

	UFUNCTION(BlueprintPure)
	bool GetIsEquipped();
	UFUNCTION(BlueprintCallable)
	void SetIsEquipped(bool bEquipped);
	UFUNCTION(BlueprintPure)
	bool GetIsReloading() const;
	UFUNCTION(BlueprintCallable)
	void SetIsReloading(bool bReloading);
	UFUNCTION(BlueprintPure)
	bool CanSell() const;
	UFUNCTION(BlueprintCallable)
	void SetCanSell(bool bSell);
	
};
