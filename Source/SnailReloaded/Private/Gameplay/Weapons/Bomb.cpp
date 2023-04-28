// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Weapons/Bomb.h"

#include "Framework/Combat/Standard/StandardCombatGameMode.h"
#include "Framework/Combat/Standard/StandardCombatGameState.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABomb::ABomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABomb::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
}

void ABomb::ExplosionTimerCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("BOOM!"));
}

// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABomb::PlantBomb(float BombTime)
{
	if(HasAuthority())
	{
		// Client_PlantBomb(BombTime);
		if(AStandardCombatGameState* CombatGameState = Cast<AStandardCombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
		{
			CombatGameState->SetPlayerDefusing(nullptr, false);
			CombatGameState->SetPlayerPlanting(nullptr, false);
			CombatGameState->OnBombPlanted();
			
		}
	}
	GetWorldTimerManager().SetTimer(ExplosionTimer, this, &ABomb::ExplosionTimerCallback, BombTime);
}

void ABomb::Client_PlantBomb_Implementation(float BombTime)
{
	PlantBomb(BombTime);
}

