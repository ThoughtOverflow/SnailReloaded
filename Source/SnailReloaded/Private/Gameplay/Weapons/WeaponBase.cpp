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
	ReloadTime = 3.f;
	bIsReloading = false;

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
	DOREPLIFETIME(AWeaponBase, ReloadTime);
	DOREPLIFETIME(AWeaponBase, bIsReloading);
	
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
	OnAmmoUpdated.Broadcast();
}

void AWeaponBase::OnRep_ClipAmmo()
{
	OnAmmoUpdated.Broadcast();
}

void AWeaponBase::OnRep_Equipped()
{
	SetActorHiddenInGame(!GetIsEquipped());
}

void AWeaponBase::OnRep_Reloading()
{
	OnReload.Broadcast();
}

void AWeaponBase::SetCurrentClipAmmo(int32 NewAmmo)
{
	if(HasAuthority())
	{
		CurrentClipAmmo = FMath::Clamp(NewAmmo, 0, MaxClipAmmo);
		OnRep_ClipAmmo();
	}
}

void AWeaponBase::SetCurrentTotalAmmo(int32 NewAmmo)
{
	if(HasAuthority())
	{
		CurrentTotalAmmo = FMath::Clamp(NewAmmo, 0, MaxTotalAmmo);
		OnRep_TotalAmmo();
	}
}

int32 AWeaponBase::GetCurrentClipAmmo()
{
	return CurrentClipAmmo;
}

int32 AWeaponBase::GetCurrentTotalAmmo()
{
	return CurrentTotalAmmo;
}

void AWeaponBase::SetMaxClipAmmo(int32 NewAmmo)
{
	MaxClipAmmo = FMath::Max(0, NewAmmo);
}

void AWeaponBase::SetMaxTotalAmmo(int32 NewAmmo)
{
	MaxTotalAmmo = FMath::Max(0, NewAmmo);
}

int32 AWeaponBase::GetMaxClipAmmo()
{
	return MaxClipAmmo;
}

int32 AWeaponBase::GetMaxTotalAmmo()
{
	return MaxTotalAmmo;
}

bool AWeaponBase::GetIsEquipped()
{
	return bIsEquipped;
}

void AWeaponBase::SetIsEquipped(bool bEquipped)
{
	if(HasAuthority())
	{
		bIsEquipped = bEquipped;
		OnRep_Equipped();
	}
}

bool AWeaponBase::GetIsReloading() const
{
	return bIsReloading;
}

void AWeaponBase::SetIsReloading(bool bReloading)
{
	if(HasAuthority())
	{
		this->bIsReloading = bReloading;
		OnRep_Reloading();
	}
}
