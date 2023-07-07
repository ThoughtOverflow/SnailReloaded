// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/SkinDisplayActor.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Framework/DefaultPlayerState.h"

// Sets default values
ASkinDisplayActor::ASkinDisplayActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseSkeleton = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DefaultSkeleton"));
	SetRootComponent(BaseSkeleton);
	HeadgearMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadgearMesh"));
	DeathEffectParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DeathParticles"));
	HeadgearMesh->SetupAttachment(GetRootComponent(), FName("headgear_socket"));
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

void ASkinDisplayActor::EquipHeadgearToDummy(USkeletalMesh* Mesh)
{
	HeadgearMesh->SetRelativeScale3D(FVector(0.1f));
	HeadgearMesh->SetRelativeLocation(FVector::ZeroVector);
	HeadgearMesh->SetRelativeRotation(FRotator::ZeroRotator);
	HeadgearMesh->SetSkeletalMesh(Mesh);
	HeadgearMesh->SetRelativeScale3D(HeadgearMesh->GetRelativeScale3D() * HeadgearMesh->GetSocketTransform(FName("mount_socket"),RTS_ParentBoneSpace).GetScale3D());
	FVector DeltaTransform = (BaseSkeleton->GetSocketLocation(FName("headgear_socket")) - HeadgearMesh->GetSocketLocation(FName("mount_socket"))) / BaseSkeleton->GetRelativeScale3D();
	HeadgearMesh->SetRelativeLocation(DeltaTransform);
	//ROTATION::

	FVector NewLoc = HeadgearMesh->GetSocketRotation(FName("mount_socket")).Quaternion().RotateVector(-DeltaTransform);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *NewLoc.ToString());
	HeadgearMesh->SetRelativeLocation(-NewLoc);
	HeadgearMesh->SetRelativeRotation(HeadgearMesh->GetSocketRotation(FName("mount_socket")));
	// FRotator DeltaRotation = HeadgearMesh->GetComponentRotation() - HeadgearMesh->GetSocketRotation(FName("mount_socket"));
	// HeadgearMesh->SetWorldRotation(DeltaRotation);
}

