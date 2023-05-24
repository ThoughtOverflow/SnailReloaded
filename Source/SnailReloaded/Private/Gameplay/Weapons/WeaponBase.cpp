// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Weapons/WeaponBase.h"

#include "NiagaraFunctionLibrary.h"
#include "Curves/CurveVector.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


FWeaponRecoil::FWeaponRecoil()
{
	bUseRecoil = true;
	RecoilResetTime = 1.f;
	FixedRecoil = nullptr;
	NumOfFixedShots = 10;
	RandomRecoilRangeMax = FVector2D::ZeroVector;
	RandomRecoilRangeMin = FVector2D::ZeroVector;
}

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	ItemIdentifier = EItemIdentifier::None;
	bShotgunSpread = false;
	HandMountSocketName = FName("weapon_socket");
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
	bCanSell = true;
	bIsOneHanded = false;

	Recoil_FiredShots = 0;
	FireAnimationDelay = 1.f;

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
	DOREPLIFETIME(AWeaponBase, bCanSell);
	
}

#if WITH_EDITOR

void AWeaponBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if(PropertyName == GET_MEMBER_NAME_CHECKED(AWeaponBase, MinimumFireDelay))
	{
		if(WeaponSlot != EWeaponSlot::Melee && WeaponMode != EWeaponMode::SemiAutomatic && MinimumFireDelay>60.f / FireRate)
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

void AWeaponBase::OnRep_CanSell()
{
	
}

void AWeaponBase::PlayFireAnimation()
{
	WeaponMesh->GetAnimInstance()->Montage_Play(GetRandomFireMontage());
	OnFireAnimPlayed();
}

void AWeaponBase::OnFireAnimPlayed()
{
	
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

bool AWeaponBase::CanSell() const
{
	return bCanSell;
}

void AWeaponBase::SetCanSell(bool bSell)
{
	if(HasAuthority())
	{
		this->bCanSell = bSell;
		OnRep_CanSell();
	}
}

void AWeaponBase::WeaponFired()
{
	if(HasAuthority())
	{
		Recoil_FiredShots++;
		Multi_OnWeaponFired();
	}
}

void AWeaponBase::ResetRecoil()
{
	if(HasAuthority())
	{
		Recoil_FiredShots = 0;
	}
}


FVector2D AWeaponBase::GetRecoilValue()
{
	
	
	FVector2D RecoilVector = FVector2D::ZeroVector;
	FVector2D RecoilAngles = FVector2D::ZeroVector;

		if(Recoil_FiredShots == 0)
		{
			return RecoilAngles;
		}
	
		if(Recoil_FiredShots > WeaponRecoil.NumOfFixedShots)
		{
			//Use randomized recoil:
			float RandX = FMath::RandRange(WeaponRecoil.RandomRecoilRangeMin.X, WeaponRecoil.RandomRecoilRangeMax.X);
			float RandY = FMath::RandRange(WeaponRecoil.RandomRecoilRangeMin.Y, WeaponRecoil.RandomRecoilRangeMax.Y);
			RecoilAngles.Set(RandX, RandY);
		}else
		{
			//Use preset recoil:
			if(WeaponRecoil.FixedRecoil && WeaponRecoil.NumOfFixedShots > 0)
			{
				RecoilAngles.Set(WeaponRecoil.FixedRecoil->GetVectorValue( static_cast<float>(Recoil_FiredShots) / static_cast<float>(WeaponRecoil.NumOfFixedShots)).X,
				WeaponRecoil.FixedRecoil->GetVectorValue(static_cast<float>(Recoil_FiredShots) / static_cast<float>(WeaponRecoil.NumOfFixedShots)).Y);
			}
		}
		//The deviation angles are the y coordinates of the unit circle.
		RecoilVector.X = FMath::Sin(FMath::DegreesToRadians(RecoilAngles.X));
		RecoilVector.Y = FMath::Sin(FMath::DegreesToRadians(RecoilAngles.Y));
	
	UE_LOG(LogTemp, Warning, TEXT("Applied recoil X: %f, - Applied recoil Y: %f"), RecoilVector.X, RecoilVector.Y);
	return RecoilVector;
}

UAnimMontage* AWeaponBase::GetRandomFireMontage()
{
	float TotalWeights = 0.f;
	TArray<float> Vals;
	FireAnimations.GenerateValueArray(Vals);
	TArray<UAnimMontage*> Keys;
	FireAnimations.GenerateKeyArray(Keys);
	
	for(auto& Val : Vals)
	{
		TotalWeights += Val;
	}
	float RandSelect = FMath::RandRange(0.f, TotalWeights);

	for(auto& AnimMontage : Keys)
	{
		float foundVal = *FireAnimations.Find(AnimMontage);
		if(RandSelect < foundVal)
		{
			return AnimMontage;
		}
		RandSelect -= foundVal;
	}
	return nullptr;
	
}

void AWeaponBase::Multi_OnWeaponFired_Implementation()
{
	//Trigger fire anim on every player:
	if(WeaponSlot != EWeaponSlot::Melee)
	{
		PlayFireAnimation();
	}
}

void AWeaponBase::SpawnBarrelParticles()
{
	if(BarrelParticleSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(BarrelParticleSystem, WeaponMesh, FName("barrel_socket"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
	}
}

void AWeaponBase::PlayFireSound()
{
	UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, FName("barrel_socket"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true, 1, 1, 0, FireSoundAttenuation);
}

void AWeaponBase::PlayEquipSound()
{
	UGameplayStatics::SpawnSoundAttached(EquipSound, WeaponMesh, FName("barrel_socket"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true, 1, 1, 0, FireSoundAttenuation);
}
