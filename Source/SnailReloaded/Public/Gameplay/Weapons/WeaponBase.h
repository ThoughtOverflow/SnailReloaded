// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class USoundCue;
USTRUCT(BlueprintType)
struct FWeaponRecoil
{
	GENERATED_BODY()

public:

	FWeaponRecoil();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseRecoil;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseRecoil"))
	float RecoilResetTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseRecoil"))
	UCurveVector* FixedRecoil;
	/**
	 * @brief Number of shots that will be accurate to the curve's preset value. (Starting from the second shot)
	 * The first shot is <b>ALWAYS</b> accurate.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseRecoil"))
	int32 NumOfFixedShots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseRecoil"))
	FVector2D RandomRecoilRangeMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseRecoil"))
	FVector2D RandomRecoilRangeMax;
	
};

UENUM(BlueprintType)
enum class EItemIdentifier : uint8
{
	None = 0,
	LightShield = 1,
	HeavyShield = 2,
	DefaultMelee = 3,
	DefaultAR = 4,
	DefaultShotgun = 5,
	NullShield = 6,
	VoltyShorty = 7,
	Triad = 8,
	Slugfire = 9,
	Snailsprayer = 10
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	None = 0,
	Primary = 1,
	Secondary = 2,
	Melee = 3
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
	FName HandMountSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
	bool bIsOneHanded;
	
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

	//Recoil:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings|Recoil", meta = (ShowOnlyInnerProperties))
	FWeaponRecoil WeaponRecoil;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
	UNiagaraSystem* ImpactParticleSystem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
	UNiagaraSystem* BarrelParticleSystem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
	UNiagaraSystem* TracerParticleSystem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
	USoundBase* FireSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
	USoundBase* EquipSound;
	
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings|Animations")
	TMap<UAnimMontage*, float> FireAnimations;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings|Animations")
	float FireAnimationDelay;
	
	
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

	UPROPERTY()
	int32 Recoil_FiredShots;

	UPROPERTY(EditDefaultsOnly)
	USoundAttenuation* FireSoundAttenuation;

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

	//Anim functions:

	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_OnWeaponFired();

	UFUNCTION()
	virtual void PlayFireAnimation();

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

	UFUNCTION(BlueprintCallable)
	virtual void WeaponFired();
	UFUNCTION(BlueprintCallable)
	void ResetRecoil();
	
	UFUNCTION(BlueprintCallable)
	FVector2D GetRecoilValue();

	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetRandomFireMontage();
	
	UFUNCTION()
	virtual void SpawnBarrelParticles();
	UFUNCTION()
	virtual void PlayFireSound();
	UFUNCTION()
	virtual void PlayEquipSound();

	UFUNCTION()
	virtual void OnFireAnimPlayed();
	
};
