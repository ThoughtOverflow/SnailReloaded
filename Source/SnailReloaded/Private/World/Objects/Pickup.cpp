// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/Pickup.h"

#include "Components/BoxComponent.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("PickupInteraction"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

	SetRootComponent(BoxCollision);
	InteractionComponent->SetupAttachment(SkeletalMesh);
	SkeletalMesh->SetupAttachment(GetRootComponent());
	SkeletalMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollision->SetSimulatePhysics(true);
	BoxCollision->SetEnableGravity(true);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	InteractionComponent->OnInteract.AddDynamic(this, &APickup::OnPickupInteract);


}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::OnPickupInteract(APawn* Interactor)
{
	if(ADefaultPlayerCharacter* DefaultPlayerCharacter = Cast<ADefaultPlayerCharacter>(Interactor))
	{
		if(DefaultPlayerCharacter->HasAuthority())
		{
			AWeaponBase* NewWpn = DefaultPlayerCharacter->AssignWeapon(WeaponClass, EEquipCondition::EquipIfStronger);
			NewWpn->SetCurrentClipAmmo(CurrentWeaponClipAmmo);
			NewWpn->SetCurrentTotalAmmo(CurrentWeaponTotalAmmo);
			Destroy();
		}
	}
}

