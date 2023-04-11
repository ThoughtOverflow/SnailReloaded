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
	SetIsReplicatedByDefault(true);
	DefaultObjectHealth = 50.f;
	SetObjectMaxHealth(100.f);
	SetObjectHealth(FDamageRequest(), DefaultObjectHealth);
	LatestDamage = FDamageResponse();
	bIsDead = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, ObjectHealth);
	DOREPLIFETIME(UHealthComponent, ObjectMaxHealth);
	DOREPLIFETIME(UHealthComponent, bIsDead);
	DOREPLIFETIME(UHealthComponent, LatestDamage);
}


void UHealthComponent::OnRep_ObjectHealth()
{
	UE_LOG(LogTemp, Warning, TEXT("Latest Damage: %f"), LatestDamage.DeltaHealth);
	ObjectHealthChanged.Broadcast(LatestDamage);
	if(bIsDead)
	{
		ObjectKilled.Broadcast(LatestDamage);
	}
}

void UHealthComponent::OnRep_Dead()
{
	ObjectKilled.Broadcast(LatestDamage);
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
		FDamageResponse DamageResponse = FDamageResponse(DamageRequest.SourceActor, DamageRequest.DeltaDamage, ObjectHealth + DamageRequest.DeltaDamage);
		LatestDamage = DamageResponse;
		this->ObjectHealth = FMath::Floor(FMath::Clamp(newHealth, 0.f, ObjectMaxHealth));
		OnRep_ObjectHealth();
		if(FMath::IsNearlyZero(ObjectHealth))
		{
			if(!bIsDead)
			{
				bIsDead = true;
				OnRep_Dead();
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
