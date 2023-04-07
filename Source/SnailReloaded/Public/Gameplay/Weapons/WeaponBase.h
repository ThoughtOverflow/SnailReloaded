// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated)
	EWeaponMode WeaponMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated)
	EWeaponSlot WeaponSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", ReplicatedUsing = OnRep_ClipAmmo)
	int32 CurrentClipAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", ReplicatedUsing = OnRep_TotalAmmo)
	int32 CurrentTotalAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated)
	int32 MaxClipAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated)
	int32 MaxTotalAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated)
	bool bShotgunSpread;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated, meta = (EditCondition = bShotgunSpread))
	int32 NumOfPellets;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated)
	float ProjectileDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated)
	float ProjectilePenetrationMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", Replicated)
	UCurveFloat* DamageDropoffCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* WeaponMesh;

	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnRep_TotalAmmo();
	UFUNCTION()
	void OnRep_ClipAmmo();

};
