// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ThumbnailHelpers.h"
#include "Components/ArmoredHealthComponent.h"
#include "Components/HealthComponent.h"
#include "Framework/DefaultGameMode.h"
#include "Framework/DefaultPlayerController.h"
#include "Framework/Combat/CombatGameMode.h"
#include "Framework/Combat/CombatPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ADefaultPlayerCharacter::ADefaultPlayerCharacter()
{
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	CameraComponent->SetupAttachment(GetMesh());
	CameraComponent->bUsePawnControlRotation = true;
	CameraComponent->SetFieldOfView(90.f);

	PlayerHealthComponent = CreateDefaultSubobject<UArmoredHealthComponent>(TEXT("PlayerHealthComponent"));
	PlayerHealthComponent->DefaultObjectHealth = 100.f;

	PrimaryWeapon = nullptr;
	SecondaryWeapon = nullptr;
	MeleeWeapon = nullptr;
	
	
}

// Called when the game starts or when spawned



void ADefaultPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADefaultPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADefaultPlayerCharacter, MeleeWeapon);
	DOREPLIFETIME(ADefaultPlayerCharacter, PrimaryWeapon);
	DOREPLIFETIME(ADefaultPlayerCharacter, SecondaryWeapon);
	DOREPLIFETIME(ADefaultPlayerCharacter, CurrentlyEquippedWeapon);
}

void ADefaultPlayerCharacter::Move(const FInputActionInstance& Action)
{
	FInputActionValue InputActionValue = Action.GetValue();
	FVector2d MoveVector = InputActionValue.Get<FVector2d>();
	
	if(MoveVector.X != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), MoveVector.X);	
	}
	
	if(MoveVector.Y != 0.f)
	{
		AddMovementInput(GetActorRightVector(), MoveVector.Y);	
	}
}

void ADefaultPlayerCharacter::Look(const FInputActionInstance& Action)
{
	FInputActionValue InputActionValue = Action.GetValue();
	FVector2d LookVector = InputActionValue.Get<FVector2d>();
	
	if(LookVector.X != 0.f)
	{
		AddControllerYawInput(LookVector.X);
	}
	
	if(LookVector.Y != 0.f)
	{
		AddControllerPitchInput(LookVector.Y);	
	}
}

void ADefaultPlayerCharacter::HealthChange(const FInputActionInstance& Action)
{
		if(CurrentlyEquippedWeapon)
		{
			UnequipWeapon();
		}else
		{
			int32 RandInt = FMath::RandRange(0,2);
			EquipWeapon(RandInt==0 ? EWeaponSlot::Melee : RandInt==1 ? EWeaponSlot::Primary : EWeaponSlot::Secondary);
		}
		return;
		FDamageRequest DamageRequest;
		DamageRequest.SourcePlayer = Cast<ACombatPlayerController>(GetController());
		DamageRequest.DeltaDamage = -15.f;
		DamageRequest.TargetActor = this;
		Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->ChangeObjectHealth(DamageRequest);
}

// Called every frame
void ADefaultPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADefaultPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	ADefaultPlayerController* PC = Cast<ADefaultPlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	EnhancedInputSubsystem->ClearAllMappings();
	EnhancedInputSubsystem->AddMappingContext(PlayerMappingContext, 0);
	
	EnhancedInputComponent->BindAction(MoveInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::Move);
	EnhancedInputComponent->BindAction(LookInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::Look);
	EnhancedInputComponent->BindAction(HP_Test, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HealthChange);
	
}

void ADefaultPlayerCharacter::OnPlayerPossessed(ADefaultPlayerController* PlayerController)
{
	if(PlayerController)
	{
		//Add wpn after possessing
		if(TestWpn)
		{
			AssignWeapon(TestWpn);
		}
		if(TestWpn2)
		{
			AssignWeapon(TestWpn2);
		}
		if(TestWpn3)
		{
			AssignWeapon(TestWpn3);
		}
	}
}

bool ADefaultPlayerCharacter::AssignWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
	if(HasAuthority())
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = this;
		AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, GetMesh()->GetSocketLocation(FName("hand_r")), FRotator::ZeroRotator, SpawnParameters);
		Weapon->bIsEquipped = false;
		Weapon->OnRep_Equipped();
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("hand_r"));
		 AWeaponBase* PrevWpn = GetWeaponAtSlot(Weapon->WeaponSlot);
		if(PrevWpn)
		{
			//Drop wpn first
		}
		switch (Weapon->WeaponSlot) {
		case EWeaponSlot::Primary: PrimaryWeapon=Weapon; break;
		case EWeaponSlot::Secondary: SecondaryWeapon=Weapon; break;
		case EWeaponSlot::Melee: MeleeWeapon=Weapon; break;
		default:;
		}
		return Weapon != nullptr;
	}else
	{
		Server_AssignWeapon(WeaponClass);
	}
	return false;
}

void ADefaultPlayerCharacter::Server_AssignWeapon_Implementation(TSubclassOf<AWeaponBase> WeaponClass)
{
	AssignWeapon(WeaponClass);
}

AWeaponBase* ADefaultPlayerCharacter::EquipWeapon(EWeaponSlot Slot)
{
	if(HasAuthority())
	{
		if(GetWeaponAtSlot(Slot) != CurrentlyEquippedWeapon)
		{
			if(CurrentlyEquippedWeapon)
			{
				UnequipWeapon();
			}
			//EquipWeapon;
			CurrentlyEquippedWeapon = GetWeaponAtSlot(Slot);
			CurrentlyEquippedWeapon->bIsEquipped = true;
			CurrentlyEquippedWeapon->OnRep_Equipped();
		}
	}else
	{
		Server_EquipWeapon(Slot);
	}
	
	return CurrentlyEquippedWeapon;
}

void ADefaultPlayerCharacter::Server_EquipWeapon_Implementation(EWeaponSlot Slot)
{
	EquipWeapon(Slot);
}

void ADefaultPlayerCharacter::UnequipWeapon()
{
	if(HasAuthority())
	{
		CurrentlyEquippedWeapon->bIsEquipped = false;
		CurrentlyEquippedWeapon->OnRep_Equipped();
		CurrentlyEquippedWeapon = nullptr;
	}else
	{
		Server_UnequipWeapon();
	}
	
}

void ADefaultPlayerCharacter::Server_UnequipWeapon_Implementation()
{
	UnequipWeapon();
}

AWeaponBase* ADefaultPlayerCharacter::GetWeaponAtSlot(EWeaponSlot Slot)
{
	AWeaponBase* Weapon;
	switch (Slot) {
	case EWeaponSlot::Primary: Weapon = PrimaryWeapon; break;
	case EWeaponSlot::Secondary: Weapon = SecondaryWeapon; break;
	case EWeaponSlot::Melee: Weapon = MeleeWeapon; break;
	default: Weapon = nullptr;
	}
	return Weapon;
}

