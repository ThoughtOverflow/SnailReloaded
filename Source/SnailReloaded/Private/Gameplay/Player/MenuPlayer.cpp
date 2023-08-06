// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Player/MenuPlayer.h"

// Sets default values
AMenuPlayer::AMenuPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	
}

// Called when the game starts or when spawned
void AMenuPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMenuPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMenuPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

