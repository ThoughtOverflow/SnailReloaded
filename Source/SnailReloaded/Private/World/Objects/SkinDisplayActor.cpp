// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/SkinDisplayActor.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

// Sets default values
ASkinDisplayActor::ASkinDisplayActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseSkeleton = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DefaultSkeleton"));
	SetRootComponent(BaseSkeleton);
	HeadgearMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadgearMesh"));
	DeathEffectParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DeathParticles"));
	HeadgearMesh->SetupAttachment(GetRootComponent(), FName("head"));
	DeathEffectParticles->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ASkinDisplayActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkinDisplayActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

