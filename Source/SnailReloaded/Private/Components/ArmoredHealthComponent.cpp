// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Components/ArmoredHealthComponent.h"

#include "Net/UnrealNetwork.h"


FShieldProperties::FShieldProperties(): ShieldHealth(0), ShieldDamageReduction(0)
{
}

FShieldProperties::FShieldProperties(float ShieldHealth, float ShieldDamageReduction): ShieldHealth(ShieldHealth),
                                                                                       ShieldDamageReduction(ShieldDamageReduction)
{
}

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
	return Super::ChangeObjectHealth(DamageRequest);
}

void UArmoredHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UArmoredHealthComponent, ShieldHealth);
	DOREPLIFETIME(UArmoredHealthComponent, ShieldDamageReduction);
}

void UArmoredHealthComponent::OnRep_ShieldHealth()
{
	//UI BIND
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
