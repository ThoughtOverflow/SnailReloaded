// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/DummyManny.h"

#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ADummyManny::ADummyManny()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(CapsuleComponent);
	CapsuleComponent->SetGenerateOverlapEvents(false);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	SkeletalMeshComponent->SetGenerateOverlapEvents(true);
	SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	bReplicates = true;
	SetReplicateMovement(true);
	
	DummyHealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("DummyHealth"));
	DummyHealthComponent->ObjectHealthChanged.AddDynamic(this, &ADummyManny::OnHealthChanged);
	DummyHealthComponent->ObjectKilled.AddDynamic(this, &ADummyManny::OnKilled);

	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("RenderComponent"));
	TextRenderComponent->SetupAttachment(SkeletalMeshComponent, FName("head"));
	TextRenderComponent->SetText(FText::FromString("Latest Damage: N/A"));

}

// Called when the game starts or when spawned
void ADummyManny::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADummyManny::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

void ADummyManny::OnKilled(FDamageResponse LatestDamage)
{
	TextRenderComponent->SetText(FText::FromString("Latest Damage: ded"));
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMeshComponent->SetSimulatePhysics(true);
	SkeletalMeshComponent->SetEnableGravity(true);
}

void ADummyManny::OnHealthChanged(FDamageResponse LatestDamage)
{
	UE_LOG(LogTemp, Warning, TEXT("Auth: %d"), HasAuthority());
	TextRenderComponent->SetText(FText::FromString(FString::Printf(TEXT("Latest Damage: %f"), LatestDamage.DeltaHealth)));
}

// Called every frame
void ADummyManny::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

