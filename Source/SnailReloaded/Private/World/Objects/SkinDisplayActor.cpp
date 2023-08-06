// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/SkinDisplayActor.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Framework/DefaultPlayerState.h"
#include "Serialization/PropertyLocalizationDataGathering.h"

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
	HeadgearMesh->SetRelativeScale3D(FVector(1.f));
	HeadgearMesh->SetRelativeLocation(FVector::ZeroVector);
	HeadgearMesh->SetRelativeRotation(FRotator::ZeroRotator);
	HeadgearMesh->SetSkeletalMesh(Mesh);
	HeadgearMesh->SetRelativeScale3D(HeadgearMesh->GetRelativeScale3D() * HeadgearMesh->GetSocketTransform(FName("mount_socket"),RTS_ParentBoneSpace).GetScale3D());
	FRotator actorDeltaRotation = GetActorRotation() - FRotator(0.f, 90.f, 0.f);
	//UE_LOG(LogTemp,	Warning, TEXT("%s AND %s"), *(BaseSkeleton->GetSocketLocation(FName("headgear_socket"))/ BaseSkeleton->GetRelativeScale3D()).ToString(), *(HeadgearMesh->GetSocketLocation(FName("mount_socket"))/ BaseSkeleton->GetRelativeScale3D()).ToString());
	// FVector DeltaTransform = (BaseSkeleton->GetSocketLocation(FName("headgear_socket")) - HeadgearMesh->GetSocketLocation(FName("mount_socket"))) / BaseSkeleton->GetRelativeScale3D();
	FVector DeltaTransform = -HeadgearMesh->GetSocketTransform(FName("mount_socket"), RTS_ParentBoneSpace).GetLocation() * HeadgearMesh->GetSocketTransform(FName("mount_socket"),RTS_ParentBoneSpace).GetScale3D();
	// DeltaTransform = actorDeltaRotation.UnrotateVector(DeltaTransform);
	HeadgearMesh->SetRelativeLocation(DeltaTransform);
	//ROTATION::
	FRotator correctedRotation = HeadgearMesh->GetSocketTransform(FName("mount_socket"), RTS_ParentBoneSpace).Rotator();// - actorDeltaRotation;
	// UE_LOG(LogTemp, Warning, TEXT("YOMOM: %s"), *correctedRotation.ToString());
	FVector NewLoc = correctedRotation.Quaternion().RotateVector(-DeltaTransform);
	// UE_LOG(LogTemp, Warning, TEXT("YOMOM: %s"), *NewLoc.ToString());
	HeadgearMesh->SetRelativeLocation(-NewLoc);
	HeadgearMesh->SetRelativeRotation(correctedRotation);
}

