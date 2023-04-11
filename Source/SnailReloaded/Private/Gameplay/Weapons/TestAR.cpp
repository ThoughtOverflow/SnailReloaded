// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Weapons/TestAR.h"

ATestAR::ATestAR()
{
	WeaponName = NSLOCTEXT("WeaponNames", "StandardAssaultRifle", "AssaultRifle");
	WeaponSlot = EWeaponSlot::Primary;
	WeaponMode = EWeaponMode::Automatic;
	CurrentClipAmmo = 10;
	MaxClipAmmo = 10;
	CurrentTotalAmmo = 90;
	MaxTotalAmmo = 100;
	ReloadTime = 3.0f;
	FireRate = 600.f;
	bUseConstantDamage = false;
	bShotgunSpread = false;
}
