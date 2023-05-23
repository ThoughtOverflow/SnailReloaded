// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/Pickup.h"

#include "Components/BoxComponent.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Gameplay/UI/WeaponPickupWidget.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickupGlobalScale = 1.f;

	bReplicates = true;
	SetReplicateMovement(true);
	
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

	WeaponReference = nullptr;


}


void APickup::OnRep_WeaponRef()
{
	SkeletalMesh->SetSkeletalMesh(WeaponReference.GetDefaultObject()->WeaponMesh->GetSkeletalMeshAsset(), false);
	SkeletalMesh->SetRelativeScale3D(WeaponReference.GetDefaultObject()->WeaponMesh->GetRelativeScale3D() * PickupGlobalScale);
	for(int i=0; i<WeaponReference.GetDefaultObject()->WeaponMesh->GetMaterials().Num(); i++)
	{
		SkeletalMesh->SetMaterial(i, WeaponReference.GetDefaultObject()->WeaponMesh->GetMaterial(i));
	}
	SetWidgetWeaponName(WeaponReference.GetDefaultObject()->WeaponName);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	
	
}

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickup, WeaponReference);
	DOREPLIFETIME(APickup, ClipAmmo);
	DOREPLIFETIME(APickup, TotalAmmo);
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
			//Drop wpn first
			DefaultPlayerCharacter->DropWeaponAtSlot(WeaponReference.GetDefaultObject()->WeaponSlot);
			
			AWeaponBase* NewWpn = DefaultPlayerCharacter->AssignWeapon(WeaponReference, EEquipCondition::EquipIfStronger);
			NewWpn->SetCurrentClipAmmo(ClipAmmo);
			NewWpn->SetCurrentTotalAmmo(TotalAmmo);
			NewWpn->SetCanSell(false);
			Destroy();
		}
	}
}

void APickup::SetWidgetWeaponName(const FText& Name)
{
	if(UWeaponPickupWidget* WeaponPickupWidget = Cast<UWeaponPickupWidget>(InteractionComponent->GetUserWidgetObject()))
	{
		WeaponPickupWidget->WeaponName = Name;
	}
}

void APickup::SetWeaponReference(TSubclassOf<AWeaponBase> WeaponRef, ADefaultPlayerCharacter* DroppingPlayer)
{
	if(HasAuthority())
	{
		WeaponReference = WeaponRef;
		ClipAmmo = DroppingPlayer->GetCurrentlyEquippedWeapon()->GetCurrentClipAmmo();
		TotalAmmo = DroppingPlayer->GetCurrentlyEquippedWeapon()->GetCurrentTotalAmmo();
		OnRep_WeaponRef();
	}
}



