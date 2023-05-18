// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Weapons/TestMelee.h"

#include "NiagaraComponent.h"

ATestMelee::ATestMelee()
{
	WeaponName = NSLOCTEXT("WeaponNames", "StandardMelee", "Knife");
	WeaponSlot = EWeaponSlot::Melee;
	bUseConstantDamage = true;
	ConstantDamage = 50.f;
	MinimumFireDelay = 2.f;

	TrailParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailParticle"));
	TrailParticleComponent->SetupAttachment(WeaponMesh);
	
}

void ATestMelee::OnWeaponFireAnimationPlayed()
{
	Super::OnWeaponFireAnimationPlayed();

	if(TrailParticleComponent)
	{
		TrailParticleComponent->ActivateSystem(true);
	}
	
	
}
