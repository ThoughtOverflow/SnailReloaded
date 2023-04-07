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
	ProjectileDamage = 10.f;
	WeaponMode = EWeaponMode::Automatic;
	WeaponSlot = EWeaponSlot::Primary;
	CurrentClipAmmo = 10.f;
	CurrentTotalAmmo = 90.f;
	MaxClipAmmo = 10.f;
	MaxTotalAmmo = 100.f;
	NumOfPellets = 1;
	ProjectilePenetrationMultiplier = 0.3f;

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
	DOREPLIFETIME(AWeaponBase, ProjectileDamage);
	DOREPLIFETIME(AWeaponBase, WeaponMode);
	DOREPLIFETIME(AWeaponBase, WeaponSlot);
	DOREPLIFETIME(AWeaponBase, CurrentClipAmmo);
	DOREPLIFETIME(AWeaponBase, CurrentTotalAmmo);
	DOREPLIFETIME(AWeaponBase, DamageDropoffCurve);
	DOREPLIFETIME(AWeaponBase, MaxClipAmmo);
	DOREPLIFETIME(AWeaponBase, MaxTotalAmmo);
	DOREPLIFETIME(AWeaponBase, NumOfPellets);
	DOREPLIFETIME(AWeaponBase, NumOfPellets);
	DOREPLIFETIME(AWeaponBase, ProjectilePenetrationMultiplier);
	
}

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

