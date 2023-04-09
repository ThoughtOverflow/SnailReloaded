// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Components/HealthComponent.h"

#include "Net/UnrealNetwork.h"


FDamageRequest::FDamageRequest(): SourceActor(nullptr), TargetActor(nullptr), DeltaDamage(0)
{
}

FDamageResponse::FDamageResponse(): SourceActor(nullptr), DeltaHealth(0), NewHealth(-1)
{
}



FDamageResponse::FDamageResponse(AActor* SrcActor, float DeltaHealth, float NewHealth):
	SourceActor(SrcActor), DeltaHealth(DeltaHealth), NewHealth(NewHealth)
{
}


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DefaultObjectHealth = 50.f;
	SetObjectMaxHealth(100.f);
	LatestDamage = FDamageResponse();
	bIsDead = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	SetObjectHealth(FDamageRequest(), DefaultObjectHealth);
	
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, ObjectHealth);
	DOREPLIFETIME(UHealthComponent, ObjectMaxHealth);
	DOREPLIFETIME(UHealthComponent, bIsDead);
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


float UHealthComponent::GetObjectHealth() const
{
	return ObjectHealth;
}

FDamageResponse UHealthComponent::ChangeObjectHealth(FDamageRequest DamageRequest)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		float newHealth = GetObjectHealth() + DamageRequest.DeltaDamage;
		SetObjectHealth(DamageRequest, newHealth);
	}
	return FDamageResponse();
}

FDamageResponse UHealthComponent::SetObjectHealth(FDamageRequest DamageRequest, float newHealth)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		if(bIsDead) return FDamageResponse();
		this->ObjectHealth = FMath::Floor(FMath::Clamp(newHealth, 0.f, ObjectMaxHealth));
		FDamageResponse DamageResponse = FDamageResponse(DamageRequest.SourceActor, DamageRequest.DeltaDamage, ObjectHealth);
		ObjectHealthChanged.Broadcast(DamageResponse);
		LatestDamage = DamageResponse;
		if(FMath::IsNearlyZero(ObjectHealth))
		{
			if(!bIsDead)
			{
				ObjectKilled.Broadcast(DamageResponse);
				bIsDead = true;
			}
		}
		return DamageResponse;
	}
	return FDamageResponse();
}

float UHealthComponent::GetObjectMaxHealth() const
{
	return ObjectMaxHealth;
}

void UHealthComponent::SetObjectMaxHealth(float newHealth)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		this->ObjectMaxHealth = newHealth;
	}
}
