// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Weapons/Bomb.h"

#include "Framework/Combat/Standard/StandardCombatGameMode.h"
#include "Framework/Combat/Standard/StandardCombatGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABomb::ABomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	BombMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BombMesh"));
	SetRootComponent(BombMesh);
	DefuseRadius = CreateDefaultSubobject<USphereComponent>(TEXT("DefuseRad"));
	DeathRadius = CreateDefaultSubobject<USphereComponent>(TEXT("KillRad"));
	DefuseRadius->SetupAttachment(GetRootComponent());
	DeathRadius->SetupAttachment(GetRootComponent());
	DefuseRadius->SetSphereRadius(75.f);
	DeathRadius->SetSphereRadius(1000.f);
	DefuseRadius->OnComponentBeginOverlap.AddDynamic(this, &ABomb::PlayerEnteredDefuse);
	DeathRadius->OnComponentBeginOverlap.AddDynamic(this, &ABomb::ABomb::PlayerEnteredDeath);
	DefuseRadius->OnComponentEndOverlap.AddDynamic(this, &ABomb::PlayerLeftDefuse);
	DeathRadius->OnComponentEndOverlap.AddDynamic(this, &ABomb::ABomb::PlayerLeftDeath);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->bInvulnerable = true;
	HealthComponent->OnTeamQuery.BindDynamic(this, &ABomb::GetBombTeam);
	
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();
	
}

EGameTeams ABomb::GetBombTeam()
{
	//Nonsense, return None;
	return EGameTeams::None;
}

void ABomb::PlayerEnteredDefuse(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(OtherActor))
	{
		if(!PlayersInDefuseRadius.Contains(PlayerCharacter))
		{
			PlayersInDefuseRadius.Add(PlayerCharacter);
		}
	}
}

void ABomb::PlayerLeftDefuse(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(OtherActor))
	{
		if(PlayersInDefuseRadius.Contains(PlayerCharacter))
		{
			PlayersInDefuseRadius.Remove(PlayerCharacter);
		}
	}
}

void ABomb::PlayerEnteredDeath(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsPendingKillPending()) return;
	if(OtherActor->GetComponentByClass(UHealthComponent::StaticClass()))
	{
		if(!PlayersInDeathRadius.Contains(OtherActor))
		{
			PlayersInDeathRadius.Add(OtherActor);
		}
	}
}

void ABomb::PlayerLeftDeath(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->IsPendingKillPending()) return;
	if(OtherActor->GetComponentByClass(UHealthComponent::StaticClass()))
	{
		if(PlayersInDeathRadius.Contains(OtherActor))
		{
			PlayersInDeathRadius.Remove(OtherActor);
		}
	}
}

void ABomb::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABomb, PlayersInDeathRadius);
	DOREPLIFETIME(ABomb, PlayersInDefuseRadius);
}

// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABomb::PlantBomb()
{
	if(HasAuthority())
	{
		if(AStandardCombatGameState* CombatGameState = Cast<AStandardCombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
		{
			CombatGameState->OnBombPlanted();
		}
	}
}

void ABomb::ExplodeBomb()
{
	if(HasAuthority())
	{
		if(ACombatGameMode* CombatGameMode = Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			for(auto& Damageable : PlayersInDeathRadius)
			{
				FDamageRequest DamageRequest = FDamageRequest::DeathDamage(this, Damageable);
				CombatGameMode->ChangeObjectHealth(DamageRequest);
			}
		}
	}
}

bool ABomb::IsPlayerInDefuseRadius(ADefaultPlayerCharacter* Player)
{
	return PlayersInDefuseRadius.Contains(Player);
}

