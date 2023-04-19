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
	PreviousShieldProperties = FShieldProperties();
	CurrentShieldProperties = FShieldProperties();
	bCanSell = true;
	bCanSellPrevious = true;
}

FDamageResponse UArmoredHealthComponent::ChangeObjectHealth(FDamageRequest DamageRequest)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		if(DamageRequest.DeltaDamage < 0.f)
		{
			SetCanSell(false);
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
	DOREPLIFETIME(UArmoredHealthComponent, PreviousShieldProperties);
	DOREPLIFETIME(UArmoredHealthComponent, bCanSell);
	DOREPLIFETIME(UArmoredHealthComponent, bCanSellPrevious);
	DOREPLIFETIME(UArmoredHealthComponent, CurrentShieldProperties);
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

void UArmoredHealthComponent::RevertToPreviousState()
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		UpdateShieldProperties(PreviousShieldProperties);
	}
}


void UArmoredHealthComponent::UpdateShieldProperties(FShieldProperties Properties)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{


		if(Properties.ShieldIdentifier != EItemIdentifier::NullShield && PreviousShieldProperties.ShieldIdentifier != Properties.ShieldIdentifier)
		{
			SetCanSell(true);
		}else
		{
			SetCanSell(false);
		}
		
		//Save prev type:
		PreviousShieldProperties = FShieldProperties(GetShieldHealth(), GetShieldDamageReduction(), GetShieldIdentifier());
		bCanSellPrevious = bCanSell;
		
		SetShieldHealth(Properties.ShieldHealth);
		SetShieldDamageReduction(Properties.ShieldDamageReduction);
		SetShieldIdentifier(Properties.ShieldIdentifier);


		CurrentShieldProperties = FShieldProperties(GetShieldHealth(), GetShieldDamageReduction(), GetShieldIdentifier());
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

bool UArmoredHealthComponent::CanSellPrevious()
{
	return bCanSellPrevious;
}
