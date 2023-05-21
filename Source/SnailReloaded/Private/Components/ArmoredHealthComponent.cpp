// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Components/ArmoredHealthComponent.h"

#include "Gameplay/Weapons/WeaponBase.h"
#include "Net/UnrealNetwork.h"


FShieldProperties::FShieldProperties(): ShieldHealth(0), ShieldMaxHealth(ShieldHealth), ShieldDamageReduction(0),
                                        ShieldIdentifier(EItemIdentifier::NullShield)
{
}

FShieldProperties::FShieldProperties(float ShieldHealth, float ShieldMaxHealth, float ShieldDamageReduction,
                                     EItemIdentifier ShieldIdentifier): ShieldHealth(ShieldHealth), ShieldMaxHealth(ShieldMaxHealth),
                                                                        ShieldDamageReduction(ShieldDamageReduction), ShieldIdentifier(ShieldIdentifier)
{
}

FShieldProperties FShieldProperties::NullShield()
{
	return FShieldProperties();
}

FShieldProperties FShieldProperties::MaxHealthShield(float SetHealth, float ShieldDamageReduction,
	EItemIdentifier ShieldIdentifier)
{
	FShieldProperties Properties = FShieldProperties(SetHealth, SetHealth, ShieldDamageReduction, ShieldIdentifier);
	return Properties;
}


UArmoredHealthComponent::UArmoredHealthComponent()
{
	ShieldHealth = 0.f;
	ShieldDamageReduction = 0.0f;
	ShieldIdentifier = EItemIdentifier::NullShield;
	StoredShield = FShieldProperties();
	bCanSell = true;
}

FDamageResponse UArmoredHealthComponent::ChangeObjectHealth(const FDamageRequest& DamageRequest)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		if(DamageRequest.DeltaDamage < 0.f)
		{
			SetCanSell(false);
			float DeltaShieldHealth = FMath::Max(FMath::Floor(DamageRequest.DeltaDamage * ShieldDamageReduction), -ShieldHealth);
			float DeltaObjectHealth = FMath::CeilToFloat(DamageRequest.DeltaDamage - DeltaShieldHealth);
			FDamageResponse DamageResponse = SetObjectHealth(DamageRequest, GetObjectHealth() + DeltaObjectHealth);
			SetShieldHealth(FMath::Max(DeltaShieldHealth + ShieldHealth, 0));
			if(GetShieldHealth() == 0.f)
			{
				//Set nullShield as current shield:
				UpdateShieldProperties(FShieldProperties());
			}
			return DamageResponse;
		}
		
	}
	return Super::ChangeObjectHealth(DamageRequest);
}

void UArmoredHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UArmoredHealthComponent, ShieldHealth);
	DOREPLIFETIME(UArmoredHealthComponent, ShieldMaxHealth);
	DOREPLIFETIME(UArmoredHealthComponent, ShieldDamageReduction);
	DOREPLIFETIME(UArmoredHealthComponent, ShieldIdentifier);
	DOREPLIFETIME(UArmoredHealthComponent, StoredShield);
	DOREPLIFETIME(UArmoredHealthComponent, bCanSell);
}

void UArmoredHealthComponent::OnRep_ShieldHealth()
{
	OnShieldHealthChanged.Broadcast();
}

void UArmoredHealthComponent::OnRep_ShieldType()
{
	
}

void UArmoredHealthComponent::SetShieldHealth(float NewHealth)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		ShieldHealth = NewHealth;
		OnRep_ShieldHealth();
	}
}

float UArmoredHealthComponent::GetShieldHealth()
{
	return ShieldHealth;
}

void UArmoredHealthComponent::SetShieldMaxHealth(float NewHealth)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		ShieldMaxHealth = NewHealth;
	}
}

float UArmoredHealthComponent::GetShieldMaxHealth()
{
	return ShieldMaxHealth;
}

void UArmoredHealthComponent::SetShieldDamageReduction(float NewReduction)
{
	
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		ShieldDamageReduction = NewReduction;
	}
}

float UArmoredHealthComponent::GetShieldDamageReduction()
{
	return ShieldDamageReduction;
}

void UArmoredHealthComponent::SetShieldIdentifier(EItemIdentifier NewIdentifier)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		ShieldIdentifier = NewIdentifier;
		OnRep_ShieldType();
	}
}

EItemIdentifier UArmoredHealthComponent::GetShieldIdentifier()
{
	return ShieldIdentifier;
}

void UArmoredHealthComponent::RevertToPreviousState()
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		UpdateShieldProperties(StoredShield);
		SetCanSell(false);
	}
}


void UArmoredHealthComponent::UpdateShieldProperties(const FShieldProperties& Properties)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		
		SetShieldHealth(Properties.ShieldHealth);
		SetShieldMaxHealth(Properties.ShieldMaxHealth);
		SetShieldDamageReduction(Properties.ShieldDamageReduction);
		SetShieldIdentifier(Properties.ShieldIdentifier);
	}
}

void UArmoredHealthComponent::StoreCurrentShieldData()
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		StoredShield = FShieldProperties(GetShieldHealth(), GetShieldMaxHealth(), GetShieldDamageReduction(), GetShieldIdentifier());
	}
}

FShieldProperties UArmoredHealthComponent::GetStoredShieldData()
{
	return StoredShield;
}

void UArmoredHealthComponent::SetCanSell(bool bSell)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		bCanSell = bSell;
	}
}

bool UArmoredHealthComponent::CanSell()
{
	return bCanSell;
}

void UArmoredHealthComponent::ResetShieldHeath()
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		UpdateShieldProperties(FShieldProperties(GetShieldHealth(), GetShieldMaxHealth(), GetShieldDamageReduction(), ShieldIdentifier));	
	}
}

float UArmoredHealthComponent::GetDamageToKill()
{
	// The current object's health should be equal to the damage taken by the object itself, aka the damage * 1 minus the Shield's reduction rate.
	//Health = Damage * (1 - ReductionRate);
	float RequiredDamage = GetObjectHealth() / (1.f - GetShieldDamageReduction());
	return -RequiredDamage;
}

