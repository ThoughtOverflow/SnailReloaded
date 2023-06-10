// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Gadgets/Gadget.h"

// Sets default values
AGadget::AGadget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGadget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGadget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

