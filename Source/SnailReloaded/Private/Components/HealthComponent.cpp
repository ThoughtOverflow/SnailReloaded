// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Components/HealthComponent.h"

#include "Framework/Combat/CombatPlayerState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Net/UnrealNetwork.h"


FGameObjectType::FGameObjectType(): bIsPlayer(false), ObjectType(EGameObjectTypes::None)
{
}

float FGameObjectType::GetDamageMultiplier(EGameObjectTypes Type)
{
	if(DamageMultipliers.Contains(Type))
	{
		return *DamageMultipliers.Find(Type);
	}
	return 0.f;
}

FDamageRequest::FDamageRequest()
{
	SourceActor = nullptr;
	TargetActor = nullptr;
	DeltaDamage = 0.f;
}

FDamageRequest FDamageRequest::DeathDamage(AActor* SourceActor, AActor* TargetActor)
{
	FDamageRequest DamageRequest = FDamageRequest();
	DamageRequest.SourceActor = SourceActor;
	DamageRequest.TargetActor = TargetActor;
	if(UHealthComponent* HealthComponent = Cast<UHealthComponent>(TargetActor->GetComponentByClass(UHealthComponent::StaticClass())))
	{
		DamageRequest.DeltaDamage = -HealthComponent->GetObjectHealth();
	}else
	{
		DamageRequest.DeltaDamage = 0.f;
	}
	return DamageRequest;
}


FDamageResponse::FDamageResponse()
{
	SourceActor = nullptr;
	DeltaHealth = 0;
	NewHealth = -1;
}



FDamageResponse::FDamageResponse(AActor* SrcActor, float DeltaHealth, float NewHealth)
{
	SourceActor = SrcActor;
	this->DeltaHealth = DeltaHealth;
	this->NewHealth = NewHealth;
}


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	DefaultObjectHealth = 50.f;
	ObjectMaxHealth = 100.f;
	ObjectHealth = DefaultObjectHealth;
	LatestDamage = FDamageResponse();
	bIsDead = false;
	bInvulnerable = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwner() && GetOwner()->HasAuthority())
	{
		SetObjectHealth(FDamageRequest(), DefaultObjectHealth);
	}
	
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

FDamageResponse UHealthComponent::ChangeObjectHealth(const FDamageRequest& DamageRequest)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		float newHealth = GetObjectHealth() + DamageRequest.DeltaDamage;
		SetObjectHealth(DamageRequest, newHealth);
	}
	return FDamageResponse();
}

FDamageResponse UHealthComponent::SetObjectHealth(const FDamageRequest& DamageRequest, float newHealth)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		if(bIsDead || bInvulnerable) return FDamageResponse();
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

float UHealthComponent::GetDamageMultiplierForTarget(UHealthComponent* TargetComp)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		EGameObjectTypes CheckType = EGameObjectTypes::None;
		if(TargetComp->GameObjectType.bIsPlayer && GameObjectType.bIsPlayer)
		{
			ADefaultPlayerCharacter* SourcePlayer = Cast<ADefaultPlayerCharacter>(GetOwner());
			ADefaultPlayerCharacter* TargetPlayer = Cast<ADefaultPlayerCharacter>(TargetComp->GetOwner());
			if(SourcePlayer && TargetPlayer)
			{
				ACombatPlayerState* SourcePlayerState = SourcePlayer->GetController()->GetPlayerState<ACombatPlayerState>();
				ACombatPlayerState* TargetPlayerState = TargetPlayer->GetController()->GetPlayerState<ACombatPlayerState>();
				if(SourcePlayerState && TargetPlayerState)
				{
					CheckType = SourcePlayerState->GetTeam() == TargetPlayerState->GetTeam() ? EGameObjectTypes::AllyPlayer : EGameObjectTypes::EnemyPlayer;
				}
				
			}
		}else
		{
			CheckType = TargetComp->GameObjectType.ObjectType;
		}
		return GameObjectType.GetDamageMultiplier(CheckType);
	}
	return 0.f;
}
