// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Components/ArmoredHealthComponent.h"


UArmoredHealthComponent::UArmoredHealthComponent()
{
	ShieldHealth = 50.f;
	ShieldDamageReduction = 0.5f;
}

FDamageResponse UArmoredHealthComponent::ChangeObjectHealth(FDamageRequest DamageRequest)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		if(DamageRequest.DeltaDamage < 0.f)
		{
			float DeltaShieldHealth = FMath::Max(FMath::Floor(DamageRequest.DeltaDamage * ShieldDamageReduction), -ShieldHealth);
			float DeltaObjectHealth = FMath::CeilToFloat(DamageRequest.DeltaDamage - DeltaShieldHealth);
			UE_LOG(LogTemp, Warning, TEXT("Total damage: %f - Shield take: %f - Body take: %f"), DamageRequest.DeltaDamage, DeltaShieldHealth, DeltaObjectHealth);
			SetObjectHealth(DamageRequest, GetObjectHealth() + DeltaObjectHealth);
			ShieldHealth = FMath::Max(DeltaShieldHealth + ShieldHealth, 0);
			UE_LOG(LogTemp, Warning, TEXT("Shield hp: %f - Body hp: %f"), ShieldHealth, GetObjectHealth());
		}
		
	}
	return FDamageResponse();
}
