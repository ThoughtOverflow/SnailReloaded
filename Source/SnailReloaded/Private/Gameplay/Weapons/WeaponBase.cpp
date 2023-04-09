// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Weapons/WeaponBase.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	
	bShotgunSpread = false;
	ConstantDamage = 10.f;
	WeaponMode = EWeaponMode::Automatic;
	WeaponSlot = EWeaponSlot::Primary;
	CurrentClipAmmo = 10.f;
	CurrentTotalAmmo = 90.f;
	BurstAmount = 3;
	MaxClipAmmo = 10.f;
	MaxTotalAmmo = 100.f;
	NumOfPellets = 1;
	ProjectilePenetrationMultiplier = 0.3f;
	bUseConstantDamage = false;
	BarrelMaxDeviation = 10.f;
	BarrelMinDeviation = 5.f;
	FireRate = 100.f;
	MinimumFireDelay = 0.05f;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeaponBase, bShotgunSpread);
	DOREPLIFETIME(AWeaponBase, ConstantDamage);
	DOREPLIFETIME(AWeaponBase, WeaponMode);
	DOREPLIFETIME(AWeaponBase, WeaponSlot);
	DOREPLIFETIME(AWeaponBase, CurrentClipAmmo);
	DOREPLIFETIME(AWeaponBase, CurrentTotalAmmo);
	DOREPLIFETIME(AWeaponBase, DamageCurve);
	DOREPLIFETIME(AWeaponBase, MaxClipAmmo);
	DOREPLIFETIME(AWeaponBase, MaxTotalAmmo);
	DOREPLIFETIME(AWeaponBase, NumOfPellets);
	DOREPLIFETIME(AWeaponBase, ProjectilePenetrationMultiplier);
	DOREPLIFETIME(AWeaponBase, bIsEquipped);
	DOREPLIFETIME(AWeaponBase, bUseConstantDamage);
	DOREPLIFETIME(AWeaponBase, BarrelMaxDeviation);
	DOREPLIFETIME(AWeaponBase, BarrelMinDeviation);
	DOREPLIFETIME(AWeaponBase, BurstAmount);
	DOREPLIFETIME(AWeaponBase, FireRate);
	
}

#if WITH_EDITOR

void AWeaponBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if(PropertyName == GET_MEMBER_NAME_CHECKED(AWeaponBase, MinimumFireDelay))
	{
		if(WeaponSlot != EWeaponSlot::Melee && MinimumFireDelay>60.f / FireRate)
		{
			MinimumFireDelay = 60.f/FireRate;
		}
	}
}

#endif

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::OnRep_TotalAmmo()
{
}

void AWeaponBase::OnRep_ClipAmmo()
{
}

void AWeaponBase::OnRep_Equipped()
{
	SetActorHiddenInGame(!bIsEquipped);
}

