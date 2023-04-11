// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Weapons/TestShotgun.h"

ATestShotgun::ATestShotgun()
{
	WeaponName = NSLOCTEXT("WeaponNames", "StandardShotgun", "Shotgun");
	WeaponSlot = EWeaponSlot::Secondary;
	WeaponMode = EWeaponMode::Burst;
	MinimumFireDelay = 0.3f;
	CurrentClipAmmo = 6;
	MaxClipAmmo = 6;
	CurrentTotalAmmo = 50;
	MaxTotalAmmo =50;
	ReloadTime = 3.f;
	BurstAmount = 3;
	FireRate = 200.f;
	bUseConstantDamage = true;
	ConstantDamage = 10.f;
	bShotgunSpread = true;
	NumOfPellets = 10;
	BarrelMinDeviation = 5.f;
	BarrelMaxDeviation = 15.f;
	
}
