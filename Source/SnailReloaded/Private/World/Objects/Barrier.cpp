// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/Barrier.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ABarrier::ABarrier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
	bBarrierActive = true;

}

// Called when the game starts or when spawned
void ABarrier::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABarrier::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABarrier, bBarrierActive);
}

void ABarrier::OnRep_Barrier()
{
	SetActorHiddenInGame(!bBarrierActive);
	SetActorEnableCollision(bBarrierActive);
}

// Called every frame
void ABarrier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABarrier::ToggleBarrier(bool bOn)
{
	if(HasAuthority() && bOn != bBarrierActive)
	{
		bBarrierActive = bOn;
		OnRep_Barrier();
	}
}

