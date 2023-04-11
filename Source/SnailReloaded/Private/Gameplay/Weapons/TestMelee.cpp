// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Weapons/TestMelee.h"

ATestMelee::ATestMelee()
{
	WeaponName = NSLOCTEXT("WeaponNames", "StandardMelee", "Knife");
	WeaponSlot = EWeaponSlot::Melee;
	bUseConstantDamage = true;
	ConstantDamage = 50.f;
}
