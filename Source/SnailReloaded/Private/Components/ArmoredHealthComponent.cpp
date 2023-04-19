// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Components/ArmoredHealthComponent.h"

#include "Gameplay/Weapons/WeaponBase.h"
#include "Net/UnrealNetwork.h"


FShieldProperties::FShieldProperties(): ShieldHealth(0), ShieldDamageReduction(0), ShieldIdentifier(EItemIdentifier::NullShield)
{
}

FShieldProperties::FShieldProperties(float ShieldHealth, float ShieldDamageReduction, EItemIdentifier ShieldIdentifier):
	ShieldHealth(ShieldHealth),
	ShieldDamageReduction(ShieldDamageReduction), ShieldIdentifier(ShieldIdentifier)
{
}

UArmoredHealthComponent::UArmoredHealthComponent()
{
	ShieldHealth = 0.f;
	ShieldDamageReduction = 0.0f;
	ShieldIdentifier = EItemIdentifier::NullShield;
	PreviousShieldType = EItemIdentifier::NullShield;
	bCanSell = true;
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
			SetShieldHealth(FMath::Max(DeltaShieldHealth + ShieldHealth, 0));
			if(GetShieldHealth() == 0.f)
			{
				//Set nullShield as current shield:
				UpdateShieldProperties(FShieldProperties());
			}
			UE_LOG(LogTemp, Warning, TEXT("Shield hp: %f - Body hp: %f"), ShieldHealth, GetObjectHealth());
		}
		
	}
	return Super::ChangeObjectHealth(DamageRequest);
}

void UArmoredHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UArmoredHealthComponent, ShieldHealth);
	DOREPLIFETIME(UArmoredHealthComponent, ShieldDamageReduction);
	DOREPLIFETIME(UArmoredHealthComponent, ShieldIdentifier);
	DOREPLIFETIME(UArmoredHealthComponent, PreviousShieldType);
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

EItemIdentifier UArmoredHealthComponent::GetPreviousShieldType()
{
	return PreviousShieldType;
}

void UArmoredHealthComponent::RevertToPreviousType()
{
	
}

void UArmoredHealthComponent::UpdateShieldProperties(FShieldProperties Properties)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{

		//Save prev type:
		PreviousShieldType = ShieldIdentifier;
		
		SetShieldHealth(Properties.ShieldHealth);
		SetShieldDamageReduction(Properties.ShieldDamageReduction);
		SetShieldIdentifier(Properties.ShieldIdentifier);
	}
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
