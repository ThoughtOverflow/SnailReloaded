// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ThumbnailHelpers.h"
#include "Components/ArmoredHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "Engine/DamageEvents.h"
#include "Framework/DefaultGameMode.h"
#include "Framework/DefaultPlayerController.h"
#include "Framework/Combat/CombatGameMode.h"
#include "Framework/Combat/CombatGameState.h"
#include "Framework/Combat/CombatPlayerController.h"
#include "Framework/Combat/CombatPlayerState.h"
#include "Framework/Combat/Standard/StandardCombatGameMode.h"
#include "Framework/Combat/Standard/StandardCombatGameState.h"
#include "Gameplay/UI/HudData.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
	PlayerHealthComponent->ObjectHealthChanged.AddDynamic(this, &ADefaultPlayerCharacter::OnHealthChanged);
	PlayerHealthComponent->ObjectKilled.AddDynamic(this, &ADefaultPlayerCharacter::OnPlayerDied);
	PlayerHealthComponent->OnShieldHealthChanged.AddDynamic(this, &ADefaultPlayerCharacter::OnShieldHealthChanged);
	PlayerHealthComponent->OnTeamQuery.BindDynamic(this, &ADefaultPlayerCharacter::QueryGameTeam);

	PrimaryWeapon = nullptr;
	SecondaryWeapon = nullptr;
	MeleeWeapon = nullptr;
	LineTraceMaxDistance = 20000.f;
	FiredRoundsPerShootingEvent = 0;
	LastFireTime = 0.f;
	bAllowAutoReload = true;

	bHasBomb = false;
	bIsInPlantZone = false;

	TimeOfLastShot = 0.f;
	
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
	DOREPLIFETIME(ADefaultPlayerCharacter, LineTraceMaxDistance);
	DOREPLIFETIME(ADefaultPlayerCharacter, FiredRoundsPerShootingEvent);
	DOREPLIFETIME(ADefaultPlayerCharacter, bAllowAutoReload);
	DOREPLIFETIME(ADefaultPlayerCharacter, bAllowPlant);
	DOREPLIFETIME(ADefaultPlayerCharacter, bHasBomb);
	DOREPLIFETIME(ADefaultPlayerCharacter, bIsInPlantZone);
}

EGameTeams ADefaultPlayerCharacter::QueryGameTeam()
{
	if(GetController())
	{
		if(ACombatPlayerState* CombatPlayerState = GetController()->GetPlayerState<ACombatPlayerState>())
		{
			return CombatPlayerState->GetTeam();
		}
	}
	return EGameTeams::None;
}

ACombatPlayerController* ADefaultPlayerCharacter::GetCombatPlayerController()
{
	if(GetController())
	{
		if(ACombatPlayerController* CombatController = Cast<ACombatPlayerController>(GetController()))
		{
			return CombatController;
		}
	}
	return nullptr;
}

void ADefaultPlayerCharacter::BlockPlayerInputs(bool bBlock)
{
	if(!HasAuthority())
	{
		Server_BlockPlayerInputs(bBlock);
	}

	ADefaultPlayerController* PC = Cast<ADefaultPlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if(EnhancedInputSubsystem)
	{
		if(bBlock)
		{
			EnhancedInputSubsystem->RemoveMappingContext(PlayerMappingContext);
		}else
		{
			EnhancedInputSubsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
	
}

void ADefaultPlayerCharacter::Server_BlockPlayerInputs_Implementation(bool bBlock)
{
	BlockPlayerInputs(bBlock);
}

void ADefaultPlayerCharacter::Move(const FInputActionInstance& Action)
{
	FInputActionValue InputActionValue = Action.GetValue();
	FVector2d MoveVector = InputActionValue.Get<FVector2d>();

	if(GetCombatPlayerController())
	{
		if(GetCombatPlayerController()->IsAnyMenuOpen()) return;
	}
	
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
		if(Action.GetValue().Get<bool>())
		{
			FDamageRequest DamageRequest;
			DamageRequest.SourceActor = this;
			DamageRequest.DeltaDamage = 15.f;
			DamageRequest.TargetActor = this;
			Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->ChangeObjectHealth(DamageRequest);

			//Do the game start as well:
			if(HasAuthority())
 			{
 				if(ACombatGameMode* CombatGameMode = Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
 				{
 					CombatGameMode->InitializeCurrentGame();
 				}
 				
 			}else
 			{
 				Server_TemporaryShit();
 			}
 			
 		}
 }


void ADefaultPlayerCharacter::Server_TemporaryShit_Implementation()
{
	if(ACombatGameMode* CombatGameMode = Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		CombatGameMode->InitializeCurrentGame();
	}
}

void ADefaultPlayerCharacter::HandleFireInput(const FInputActionInstance& Action)
{
	
	bool ShootingInput = Action.GetValue().Get<bool>();

	if(GetCombatPlayerController())
	{
		if(GetCombatPlayerController()->IsAnyMenuOpen()) return;
	}
	
	if(ShootingInput)
	{
		//Begin Shooting
		BeginShooting();
	}else
	{
		// End Shooting
		EndShooting();
	}
}

void ADefaultPlayerCharacter::HandleReloadInput(const FInputActionInstance& Action)
{

	if(GetCombatPlayerController())
	{
		if(GetCombatPlayerController()->IsAnyMenuOpen()) return;
	}
	
	if(Action.GetValue().Get<bool>())
	{
		// Do other input checks maybe?
		StartReload();
	}
}

void ADefaultPlayerCharacter::HandleSelectPrimaryInput(const FInputActionInstance& Action)
{
	if(Action.GetValue().Get<bool>())
	{
		EquipWeapon(EWeaponSlot::Primary);
	}
}

void ADefaultPlayerCharacter::HandleSelectSecondaryInput(const FInputActionInstance& Action)
{
	if(Action.GetValue().Get<bool>())
	{
		EquipWeapon(EWeaponSlot::Secondary);
	}
}

void ADefaultPlayerCharacter::HandleSelectMeleeInput(const FInputActionInstance& Action)
{
	if(Action.GetValue().Get<bool>())
	{
		EquipWeapon(EWeaponSlot::Melee);	
	}
}

void ADefaultPlayerCharacter::HandleToggleBuyMenu(const FInputActionInstance& Action)
{
	if(Action.GetValue().Get<bool>())
	{
		if(IsLocallyControlled() && GetCombatPlayerController())
		{
			//Check for game phase:
			if(ACombatGameState* CombatGameState = Cast<ACombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
			{
				if(CombatGameState->GetCurrentGamePhase().GamePhase == EGamePhase::Preparation)
				{
					GetCombatPlayerController()->ToggleBuyMenu(true);
				}	
			}
			
		}
	}
}

void ADefaultPlayerCharacter::HandlePlantBomb(const FInputActionInstance& Action)
{
	if(Action.GetValue().Get<bool>())
	{
		TryStartPlanting();
		
	}else
	{
		TryStopPlanting();
	}
}




void ADefaultPlayerCharacter::OnReloadComplete()
{
	if(HasAuthority() && CurrentlyEquippedWeapon)
	{
		//Set new ammo amount:
		float ClipAddAmount = FMath::Min(CurrentlyEquippedWeapon->GetMaxClipAmmo() - CurrentlyEquippedWeapon->GetCurrentClipAmmo(), CurrentlyEquippedWeapon->GetCurrentTotalAmmo());
		CurrentlyEquippedWeapon->SetCurrentClipAmmo(CurrentlyEquippedWeapon->GetCurrentClipAmmo() + ClipAddAmount);
		CurrentlyEquippedWeapon->SetCurrentTotalAmmo(CurrentlyEquippedWeapon->GetCurrentTotalAmmo() - ClipAddAmount);
		CurrentlyEquippedWeapon->SetIsReloading(false);
	}
}

void ADefaultPlayerCharacter::StartReload()
{
	if(!HasAuthority())
	{
		Server_StartReload();
	}
	if(CurrentlyEquippedWeapon)
	{
		if(CurrentlyEquippedWeapon->GetCurrentTotalAmmo() == 0) return;
		if(CanPlayerReload())
		{
			if(HasAuthority())
			{
				//Resets the burst combo count:
				EndShooting();
				//---
				CurrentlyEquippedWeapon->SetIsReloading(true);
			}
			GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &ADefaultPlayerCharacter::OnReloadComplete, CurrentlyEquippedWeapon->ReloadTime);
		}
	}
}

void ADefaultPlayerCharacter::Server_StartReload_Implementation()
{
	StartReload();
}

void ADefaultPlayerCharacter::Client_StartReload_Implementation()
{
	StartReload();
}

void ADefaultPlayerCharacter::CancelReload()
{
	if(HasAuthority() && CurrentlyEquippedWeapon)
	{
		CurrentlyEquippedWeapon->SetIsReloading(false);
		GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	}
}

void ADefaultPlayerCharacter::OnRep_CurrentWeapon()
{

	if(CurrentlyEquippedWeapon)
	{
		CurrentlyEquippedWeapon->OnAmmoUpdated.Clear();
		CurrentlyEquippedWeapon->OnAmmoUpdated.AddDynamic(this, &ADefaultPlayerCharacter::OnCurrentWeaponAmmoChanged);
		CurrentlyEquippedWeapon->OnReload.Clear();
		CurrentlyEquippedWeapon->OnReload.AddDynamic(this, &ADefaultPlayerCharacter::OnCurrentWeaponReloading);
	}
	
	if(ACombatPlayerController* PlayerController = Cast<ACombatPlayerController>(GetController()))
	{
		if(IsLocallyControlled())
		{
			UHudData* HudData = PlayerController->GetHudData();
			if(CurrentlyEquippedWeapon)
			{
				HudData->SetCurrentWeaponName(CurrentlyEquippedWeapon->WeaponName)->
				SetCurrentClipAmmo(CurrentlyEquippedWeapon->GetCurrentClipAmmo())->
				SetCurrentTotalAmmo(CurrentlyEquippedWeapon->GetCurrentTotalAmmo());
					
			}else {
				HudData->SetCurrentWeaponName(FText::FromString(""))->
				SetCurrentClipAmmo(0)->
				SetCurrentTotalAmmo(0);
			}
			HudData->Submit();
		}
	}
}

void ADefaultPlayerCharacter::OnHealthChanged(const FDamageResponse& DamageResponse)
{
	if(ACombatPlayerController* PlayerController = Cast<ACombatPlayerController>(GetController()))
	{
		if(IsLocallyControlled())
		{
			PlayerController->GetHudData()->SetPlayerHealthPercentage(PlayerHealthComponent->GetObjectHealth() / PlayerHealthComponent->GetObjectMaxHealth())->
			SetPlayerShieldHealth(PlayerHealthComponent->GetShieldHealth())->
			Submit();
		}
	}
}

void ADefaultPlayerCharacter::OnPlayerDied(const FDamageResponse& DamageResponse)
{
	if(IsLocallyControlled())
	{
		BlockPlayerInputs(true);
	}
	if(HasAuthority())
	{
		//TEMP:!!
		UnequipWeapon();
		if(PrimaryWeapon) PrimaryWeapon->Destroy();
		if(SecondaryWeapon) SecondaryWeapon->Destroy();
		if(MeleeWeapon) MeleeWeapon->Destroy();
		GetCombatPlayerController()->ShowDeathScreen(true);
		GetCombatPlayerController()->SelectOverviewCamera();
		if(ACombatGameMode* CombatGameMode = Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			CombatGameMode->ProcessPlayerDeath(Cast<ACombatPlayerState>(GetPlayerState()));
		}
		this->Destroy();
	}
}

void ADefaultPlayerCharacter::OnShieldHealthChanged()
{
	if(ACombatPlayerController* PlayerController = Cast<ACombatPlayerController>(GetController()))
	{
		if(IsLocallyControlled())
		{
			PlayerController->GetHudData()->SetPlayerShieldHealth(PlayerHealthComponent->GetShieldHealth())->Submit();
		}
	}
}

void ADefaultPlayerCharacter::OnCurrentWeaponAmmoChanged()
{
	if(ACombatPlayerController* PlayerController = Cast<ACombatPlayerController>(GetController()))
	{
		if(IsLocallyControlled() && CurrentlyEquippedWeapon)
		{
			PlayerController->GetHudData()->SetCurrentClipAmmo(CurrentlyEquippedWeapon->GetCurrentClipAmmo())->
			SetCurrentTotalAmmo(CurrentlyEquippedWeapon->GetCurrentTotalAmmo())->
			Submit();
		}
	}
}

void ADefaultPlayerCharacter::OnCurrentWeaponReloading()
{
	if(ACombatPlayerController* PlayerController = Cast<ACombatPlayerController>(GetController()))
	{
		if(IsLocallyControlled() && CurrentlyEquippedWeapon)
		{
			PlayerController->GetHudData()->SetReloading(CurrentlyEquippedWeapon->GetIsReloading())->Submit();
		}
	}
}

bool ADefaultPlayerCharacter::CanPlayerReload()
{
		bool bCanReload = true;
		if(CurrentlyEquippedWeapon)
		{
			//Is weapon already being reloaded?
			bCanReload &= !CurrentlyEquippedWeapon->GetIsReloading();
			bCanReload &= CurrentlyEquippedWeapon->GetCurrentClipAmmo() != CurrentlyEquippedWeapon->GetMaxClipAmmo();
		}
		if(AStandardCombatGameState* StandardCombatGameState = Cast<AStandardCombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
		{
			//If we can plant / defuse in the game, are we?
			bCanReload &= !StandardCombatGameState->IsPlayerPlanting(this);
			bCanReload &= !StandardCombatGameState->IsPlayerDefusing(this);
		}
		return bCanReload;
}

bool ADefaultPlayerCharacter::CanPlayerAttack()
{
	bool bCanShoot = true;
	if(AStandardCombatGameState* StandardCombatGameState = Cast<AStandardCombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
    		bCanShoot &= !StandardCombatGameState->IsPlayerPlanting(this);
			bCanShoot &= !StandardCombatGameState->IsPlayerDefusing(this);
	}
	bCanShoot &= CanWeaponFireInMode();
	return bCanShoot;
}

void ADefaultPlayerCharacter::OnRep_AllowPlant()
{
	//Show plant message:
	if(GetCombatPlayerController())
	{
		GetCombatPlayerController()->GetHudData()->SetShowPlantHint(bAllowPlant)->Submit();
	}
}

void ADefaultPlayerCharacter::OnRep_HasBomb()
{
	
}


void ADefaultPlayerCharacter::CalculateWeaponRecoil(FVector& RayEndLocation)
{
	if(HasAuthority() && GetCurrentlyEquippedWeapon() && GetCurrentlyEquippedWeapon()->WeaponRecoil.bUseRecoil)
	{
			
		if(GetWorld()->TimeSince(TimeOfLastShot) > GetCurrentlyEquippedWeapon()->WeaponRecoil.RecoilResetTime)
		{
			GetCurrentlyEquippedWeapon()->ResetRecoil();
		}

		FVector2D RecoilUnitVector = GetCurrentlyEquippedWeapon()->GetRecoilValue();

		TimeOfLastShot = GetWorld()->GetTimeSeconds();
		
		FVector RecoilActualVector = RecoilActualVector = UKismetMathLibrary::GetRightVector(GetController()->GetControlRotation()) * RecoilUnitVector.X * LineTraceMaxDistance +
			UKismetMathLibrary::GetUpVector(GetController()->GetControlRotation()) * RecoilUnitVector.Y * LineTraceMaxDistance;

		RayEndLocation += RecoilActualVector;
	}
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
	EnhancedInputSubsystem->AddMappingContext(PlayerMappingContext, 0);
	
	EnhancedInputComponent->BindAction(MoveInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::Move);
	EnhancedInputComponent->BindAction(LookInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::Look);
	EnhancedInputComponent->BindAction(HP_Test, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HealthChange);
	EnhancedInputComponent->BindAction(FireInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HandleFireInput);
	EnhancedInputComponent->BindAction(FireInput, ETriggerEvent::Completed, this, &ADefaultPlayerCharacter::HandleFireInput);
	EnhancedInputComponent->BindAction(ReloadInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HandleReloadInput);
	EnhancedInputComponent->BindAction(SelectPrimaryInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HandleSelectPrimaryInput);
	EnhancedInputComponent->BindAction(SelectSecondaryInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HandleSelectSecondaryInput);
	EnhancedInputComponent->BindAction(SelectMeleeInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HandleSelectMeleeInput);
	EnhancedInputComponent->BindAction(ToggleBuyMenu, ETriggerEvent::Started, this, &ADefaultPlayerCharacter::HandleToggleBuyMenu);
	EnhancedInputComponent->BindAction(PlantBomb, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HandlePlantBomb);
	EnhancedInputComponent->BindAction(PlantBomb, ETriggerEvent::Completed, this, &ADefaultPlayerCharacter::HandlePlantBomb);
	
}

void ADefaultPlayerCharacter::OnPlayerPossessed(ACombatPlayerController* PlayerController)
{
	if(PlayerController)
	{
		//Add wpn after possessing
		if(TestWpn)
		{
			AssignWeapon(TestWpn);
		}

		//Load Default hud for player UI
		Client_LoadDefaultHudData();
		
	}
}


AWeaponBase* ADefaultPlayerCharacter::AssignWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
	if(HasAuthority())
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = this;
		AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, FVector(0.f, 0.f, 0.f), FRotator(0.f, 90.f, 0.f), SpawnParameters);
		Weapon->SetIsEquipped(false);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("hand_r"));
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
		return Weapon;
	}else
	{
		Server_AssignWeapon(WeaponClass);
	}
	return nullptr;
}

bool ADefaultPlayerCharacter::RemoveWeapon(EWeaponSlot Slot)
{
	if(HasAuthority())
	{

		if(AWeaponBase* WeaponBase = GetWeaponAtSlot(Slot))
		{
			if(GetCurrentlyEquippedWeapon() == WeaponBase)
			{
				UnequipWeapon();
			}
			WeaponBase->Destroy();
			switch (Slot) { case EWeaponSlot::None: break;
			case EWeaponSlot::Primary: PrimaryWeapon = nullptr; break;
			case EWeaponSlot::Secondary: SecondaryWeapon = nullptr; break;
			case EWeaponSlot::Melee: MeleeWeapon = nullptr; break;
			default: ; }
			EquipStrongestWeapon();
			return true;
		}
		
	}else
	{
		Server_RemoveWeapon(Slot);
	}
	return false;
}

void ADefaultPlayerCharacter::Server_RemoveWeapon_Implementation(EWeaponSlot Slot)
{
	RemoveWeapon(Slot);
}


void ADefaultPlayerCharacter::Server_AssignWeapon_Implementation(TSubclassOf<AWeaponBase> WeaponClass)
{
	AssignWeapon(WeaponClass);
}

AWeaponBase* ADefaultPlayerCharacter::EquipWeapon(EWeaponSlot Slot)
{
	if(HasAuthority())
	{
		if(GetWeaponAtSlot(Slot) != CurrentlyEquippedWeapon && GetWeaponAtSlot(Slot))
		{
			if(CurrentlyEquippedWeapon)
			{
				UnequipWeapon();
			}
			//EquipWeapon;
			SetCurrentlyEqippedWeapon(GetWeaponAtSlot(Slot));
			CurrentlyEquippedWeapon->SetIsEquipped(true);
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
		if(!CurrentlyEquippedWeapon) return;
		if(CurrentlyEquippedWeapon->GetIsReloading())
		{
			CancelReload();
		}
		CurrentlyEquippedWeapon->SetIsEquipped(false);
		SetCurrentlyEqippedWeapon(nullptr);
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

void ADefaultPlayerCharacter::BeginShooting()
{
	if(!HasAuthority())
	{
		Server_BeginShooting();
	}
	//Double thread - both Auth and Remote;

	if (CurrentlyEquippedWeapon)
	{
		
		if (CurrentlyEquippedWeapon->WeaponSlot != EWeaponSlot::Melee)
		{
			//Shootus
			FireEquippedWeapon();
		}
		else
		{
			//Do melee;
			UseMeleeWeapon();
		}
	}
	
	
}

void ADefaultPlayerCharacter::Server_BeginShooting_Implementation()
{
	BeginShooting();
}

void ADefaultPlayerCharacter::EndShooting()
{
	if(!HasAuthority())
	{
		Server_EndShooting();
	}

	if(HasAuthority())
	{
		//Reset fired combo;
		FiredRoundsPerShootingEvent = 0;
	}
	
}

void ADefaultPlayerCharacter::UseMeleeWeapon()
{
	if (HasAuthority() && GetController() && !IsPendingKillPending())
	{
		FHitResult HitResult;
		FVector TraceStartLoc = CameraComponent->GetComponentLocation();
		FVector TraceEndLoc = TraceStartLoc + GetController()->GetControlRotation().Vector() * 100.f;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		if(CanPlayerAttack())
		{
			FiredRoundsPerShootingEvent++;
			
			if(GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLoc, TraceEndLoc,ECC_Visibility, QueryParams))
			{
				Multi_SpawnImpactParticles(HitResult.ImpactPoint, HitResult.ImpactNormal);
				if(UHealthComponent* HealthComponent = Cast<UHealthComponent>(HitResult.GetActor()->GetComponentByClass(UHealthComponent::StaticClass())))
				{
					if(!HealthComponent->bIsDead)
					{
						FDamageRequest DamageRequest = FDamageRequest();
						DamageRequest.SourceActor = this;
						DamageRequest.TargetActor = HitResult.GetActor();
						DamageRequest.DeltaDamage = -GetCurrentlyEquippedWeapon()->ConstantDamage;

						Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->ChangeObjectHealth(DamageRequest);
					}
				}
			}
		}
	}
}

void ADefaultPlayerCharacter::FireEquippedWeapon()
{
	if (HasAuthority() && GetController() && !IsPendingKillPending())
	{
		FHitResult HitResult;
		FVector TraceStartLoc = CameraComponent->GetComponentLocation();
		FVector TraceEndLoc;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		//Auto reload:
		if(!CurrentlyEquippedWeapon->GetIsReloading() && CurrentlyEquippedWeapon->GetCurrentClipAmmo() == 0 && CurrentlyEquippedWeapon->GetCurrentTotalAmmo() != 0 && bAllowAutoReload)
		{
			Client_StartReload();
			return;
		}
		
		if (CurrentlyEquippedWeapon->bShotgunSpread)
		{
			QueryParams.AddIgnoredActor(this);
			//Can Shoot:
			if (CanPlayerAttack() && WeaponHasAmmo())
			{
				CancelReload();
				//Add to Combo counter
				FiredRoundsPerShootingEvent++;
				CurrentlyEquippedWeapon->SetCurrentClipAmmo(CurrentlyEquippedWeapon->GetCurrentClipAmmo()-1);
				if(CurrentlyEquippedWeapon->CanSell())
				{
					CurrentlyEquippedWeapon->SetCanSell(false);
				}
				
				float MaxEndDeviation = FMath::Tan(
					FMath::DegreesToRadians(CurrentlyEquippedWeapon->BarrelMaxDeviation / 2)) * LineTraceMaxDistance;
				float MinEndDeviation = FMath::Tan(
					FMath::DegreesToRadians(CurrentlyEquippedWeapon->BarrelMinDeviation / 2)) * LineTraceMaxDistance;
				for (int i = 0; i < CurrentlyEquippedWeapon->NumOfPellets; i++)
				{
					TraceEndLoc = TraceStartLoc + GetController()->GetControlRotation().Vector() *
						LineTraceMaxDistance;


					float DeviationDistance = FMath::RandRange(MinEndDeviation, MaxEndDeviation);
					float DeviationAngle = FMath::RandRange(0.f, 2 * PI);
					FVector2d DeviationVector = FVector2d(FMath::Cos(DeviationAngle), FMath::Sin(DeviationAngle)) *
						DeviationDistance;

					FVector EndDeviation = (UKismetMathLibrary::GetRightVector(GetController()->GetControlRotation()) *
							DeviationVector.X) +
						(UKismetMathLibrary::GetUpVector(GetController()->GetControlRotation()) * DeviationVector.Y);
					

					TraceEndLoc += EndDeviation;
					Multi_SpawnBulletParticles(TraceStartLoc, TraceEndLoc);

					CalculateWeaponRecoil(TraceEndLoc);
					
					if (GetWorld() && GetWorld()->LineTraceSingleByChannel(
						HitResult, TraceStartLoc, TraceEndLoc, ECC_Visibility, QueryParams))
					{
				
						//Process hit results:
						if(HitResult.GetActor())
						{
							Multi_SpawnImpactParticles(HitResult.ImpactPoint, HitResult.ImpactNormal);
							// DrawDebugLine(GetWorld(), TraceStartLoc, HitResult.ImpactPoint, FColor::Magenta, true, -1, 0,
							//               3);
							if(UHealthComponent* HealthComponent = Cast<UHealthComponent>(HitResult.GetActor()->GetComponentByClass(UHealthComponent::StaticClass())))
							{
								if(!HealthComponent->bIsDead)
								{
									FDamageRequest DamageRequest = FDamageRequest();
									DamageRequest.SourceActor = this;
									DamageRequest.TargetActor = HitResult.GetActor();
									DamageRequest.DeltaDamage = CurrentlyEquippedWeapon->bUseConstantDamage ?
																	-CurrentlyEquippedWeapon->ConstantDamage :
																	- FMath::RoundToFloat(CurrentlyEquippedWeapon->DamageCurve->GetFloatValue((HitResult.ImpactPoint - TraceStartLoc).Size()/100.f));
								
									Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->ChangeObjectHealth(DamageRequest);
								}
								
							}
						}
					}
				}
				GetCurrentlyEquippedWeapon()->WeaponFired();
			}
		}
		else
		{
			TraceEndLoc = TraceStartLoc + GetController()->GetControlRotation().Vector() * LineTraceMaxDistance;
			
			
			//Can Shoot:
			if (CanPlayerAttack() && WeaponHasAmmo())
			{
				CancelReload();
				//Add to Combo counter
				FiredRoundsPerShootingEvent++;
				CurrentlyEquippedWeapon->SetCurrentClipAmmo(CurrentlyEquippedWeapon->GetCurrentClipAmmo() - 1);
				if(CurrentlyEquippedWeapon->CanSell()) CurrentlyEquippedWeapon->SetCanSell(false);
				Multi_SpawnBulletParticles(TraceStartLoc, TraceEndLoc);

				CalculateWeaponRecoil(TraceEndLoc);
				CurrentlyEquippedWeapon->WeaponFired();
				
				if (GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLoc, TraceEndLoc,
				                                                       ECC_Visibility, QueryParams))
				{
					//Process hit results:
					if(HitResult.GetActor())
					{
						Multi_SpawnImpactParticles(HitResult.ImpactPoint, HitResult.ImpactNormal);
						// DrawDebugLine(GetWorld(), TraceStartLoc, HitResult.ImpactPoint, FColor::Magenta, true, -1, 0, 3);
						if(UHealthComponent* HealthComponent = Cast<UHealthComponent>(HitResult.GetActor()->GetComponentByClass(UHealthComponent::StaticClass())))
						{
							if(!HealthComponent->bIsDead)
							{
								FDamageRequest DamageRequest = FDamageRequest();
								DamageRequest.SourceActor = this;
								DamageRequest.TargetActor = HitResult.GetActor();
								DamageRequest.DeltaDamage = CurrentlyEquippedWeapon->bUseConstantDamage ?
																-CurrentlyEquippedWeapon->ConstantDamage :
																- FMath::RoundToFloat(CurrentlyEquippedWeapon->DamageCurve->GetFloatValue((HitResult.ImpactPoint - TraceStartLoc).Size() / 100.f));
				
								Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->ChangeObjectHealth(DamageRequest);
							}
							
						}
					}
					
				}
			}
		}
	}
}

bool ADefaultPlayerCharacter::CanWeaponFireInMode()
{
	if(CurrentlyEquippedWeapon)
	{
		// bool bRequiredTimeElapsed = GetWorld()->TimeSince(LastFireTime) >= FMath::Max(CurrentlyEquippedWeapon->MinimumFireDelay, WeaponFireDelay);

		

		//If the weapon is triggered for the first time in a combo (First round after mouse press), only check the MinimumDelay requirement
		bool bRequiredTimeElapsed = GetWorld()->TimeSince(LastFireTime) >= CurrentlyEquippedWeapon->MinimumFireDelay || LastFireTime == 0.f;

		//Check if it is a melee weapon - the elapsed time check can be used as a time between melee attacks check as well.
		if(CurrentlyEquippedWeapon->WeaponSlot == EWeaponSlot::Melee)
		{
			bRequiredTimeElapsed &= FiredRoundsPerShootingEvent<1;
		}else
		{
			//If the weapon has already been firing at least one round, check the actual fire rate for correction
			float WeaponFireDelay = 60 / CurrentlyEquippedWeapon->FireRate;
			if(FiredRoundsPerShootingEvent>=1) bRequiredTimeElapsed &= GetWorld()->TimeSince(LastFireTime) >= WeaponFireDelay;
		
			if(bRequiredTimeElapsed) LastFireTime = GetWorld()->TimeSeconds;
			switch (CurrentlyEquippedWeapon->WeaponMode)
			{
			case EWeaponMode::Automatic: break;
			case EWeaponMode::SemiAutomatic: bRequiredTimeElapsed &= FiredRoundsPerShootingEvent<1; break;
			case EWeaponMode::Burst: bRequiredTimeElapsed &= FiredRoundsPerShootingEvent<CurrentlyEquippedWeapon->BurstAmount; break;
			default: bRequiredTimeElapsed = false;
			}
		}
		if(bRequiredTimeElapsed) LastFireTime = GetWorld()->TimeSeconds;
		return bRequiredTimeElapsed;
	}
	return false;
}

bool ADefaultPlayerCharacter::WeaponHasAmmo()
{
	return CurrentlyEquippedWeapon != nullptr ? CurrentlyEquippedWeapon->GetCurrentClipAmmo() > 0 : false;
}

AWeaponBase* ADefaultPlayerCharacter::GetCurrentlyEquippedWeapon()
{
	return CurrentlyEquippedWeapon;
}

void ADefaultPlayerCharacter::SetCurrentlyEqippedWeapon(AWeaponBase* NewWeapon)
{
	if(HasAuthority())
	{
		CurrentlyEquippedWeapon = NewWeapon;
		OnRep_CurrentWeapon();
	}
}

float ADefaultPlayerCharacter::GetReloadProgress()
{
	if(CurrentlyEquippedWeapon && CurrentlyEquippedWeapon->GetIsReloading())
	{
		return GetWorldTimerManager().GetTimerElapsed(ReloadTimerHandle) / CurrentlyEquippedWeapon->ReloadTime;	
	}
	return 0.f;
}

AWeaponBase* ADefaultPlayerCharacter::EquipStrongestWeapon()
{
	AWeaponBase* StrongestWeapon = nullptr;
	if(PrimaryWeapon)
	{
		StrongestWeapon = PrimaryWeapon;
	}else if(SecondaryWeapon)
	{
		StrongestWeapon = SecondaryWeapon;
	}else if(MeleeWeapon)
	{
		StrongestWeapon = MeleeWeapon;
	}
	EquipWeapon(StrongestWeapon->WeaponSlot);
	return StrongestWeapon;
}

void ADefaultPlayerCharacter::TryPurchaseItem(EItemIdentifier ItemIdentifier)
{
	if(HasAuthority())
	{
		if(ACombatGameMode* CombatGameMode = Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			CombatGameMode->PurchaseItem(this, ItemIdentifier);
		}
	}else
	{
		Server_TryPurchaseItem(ItemIdentifier);
	}
}

void ADefaultPlayerCharacter::TrySellItem(EItemIdentifier ItemIdentifier)
{
	if(HasAuthority())
	{
		if(ACombatGameMode* CombatGameMode = Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			CombatGameMode->SellItem(this, ItemIdentifier);
		}
	}else
	{
		Server_TrySellItem(ItemIdentifier);
	}
}

void ADefaultPlayerCharacter::UpdateShieldProperties(FShieldProperties ShieldProperties)
{
	if(HasAuthority())
	{
		PlayerHealthComponent->UpdateShieldProperties(ShieldProperties);
	}
}

void ADefaultPlayerCharacter::StoreCurrentShieldData()
{
	if(HasAuthority() && PlayerHealthComponent)
	{
		PlayerHealthComponent->StoreCurrentShieldData();
	}
}

bool ADefaultPlayerCharacter::HasStoredShield()
{
	if(HasAuthority() && PlayerHealthComponent)
	{
		return PlayerHealthComponent->GetStoredShieldData().ShieldIdentifier != EItemIdentifier::NullShield;
	}
	return false;
}

TArray<EItemIdentifier> ADefaultPlayerCharacter::GetAllItems()
{
	TArray<EItemIdentifier> Identifiers;
	if(PrimaryWeapon) Identifiers.Add(PrimaryWeapon->ItemIdentifier);
	if(SecondaryWeapon) Identifiers.Add(SecondaryWeapon->ItemIdentifier);
	if(MeleeWeapon) Identifiers.Add(MeleeWeapon->ItemIdentifier);
	if(PlayerHealthComponent->GetShieldIdentifier() != EItemIdentifier::NullShield) Identifiers.Add(PlayerHealthComponent->GetShieldIdentifier());
	return Identifiers;
}

EWeaponSlot ADefaultPlayerCharacter::GetWeaponSlotByIdentifier(EItemIdentifier Identifier)
{
	if(PrimaryWeapon && PrimaryWeapon->ItemIdentifier == Identifier) return PrimaryWeapon->WeaponSlot;
	if(SecondaryWeapon && SecondaryWeapon->ItemIdentifier == Identifier) return SecondaryWeapon->WeaponSlot;
	if(MeleeWeapon && MeleeWeapon->ItemIdentifier == Identifier) return MeleeWeapon->WeaponSlot;
	return EWeaponSlot::None;
}

EItemIdentifier ADefaultPlayerCharacter::GetCurrentShieldType()
{
	if(PlayerHealthComponent)
	{
		return PlayerHealthComponent->GetShieldIdentifier();
	}
	return EItemIdentifier::NullShield;
}

void ADefaultPlayerCharacter::RevertToPreviousShield()
{
	if(PlayerHealthComponent && HasAuthority())
	{
		PlayerHealthComponent->RevertToPreviousState();
	}
}


bool ADefaultPlayerCharacter::CanSellCurrentShield()
{
	if(PlayerHealthComponent)
	{
		return PlayerHealthComponent->CanSell();
	}
	return false;
}

void ADefaultPlayerCharacter::SetCanSellCurrentShield(bool bSell)
{
	if(HasAuthority() && PlayerHealthComponent)
	{
		PlayerHealthComponent->SetCanSell(bSell);
	}
}

void ADefaultPlayerCharacter::Server_TrySellItem_Implementation(EItemIdentifier ItemIdentifier)
{
	TrySellItem(ItemIdentifier);
}

void ADefaultPlayerCharacter::Server_TryPurchaseItem_Implementation(EItemIdentifier ItemIdentifier)
{
	TryPurchaseItem(ItemIdentifier);
}

void ADefaultPlayerCharacter::Client_LoadDefaultHudData_Implementation()
{
	if(ACombatPlayerController* PlayerController = Cast<ACombatPlayerController>(GetController()))
	{
		if(PlayerController->PlayerHud)
		{
			UHudData* HudData = PlayerController->GetHudData();
			HudData->SetPlayerHealthPercentage(PlayerHealthComponent->GetObjectHealth() / PlayerHealthComponent->GetObjectMaxHealth());
			HudData->SetPlayerShieldHealth(PlayerHealthComponent->GetShieldHealth());
			if(CurrentlyEquippedWeapon)
			{
				HudData->SetCurrentClipAmmo(CurrentlyEquippedWeapon->GetCurrentClipAmmo())->SetCurrentTotalAmmo(CurrentlyEquippedWeapon->GetCurrentTotalAmmo())->SetCurrentWeaponName(CurrentlyEquippedWeapon->WeaponName);
			}
			HudData->Submit();
		}
	}
}

void ADefaultPlayerCharacter::Multi_SpawnImpactParticles_Implementation(FVector Loc, FVector SurfaceNormal)
{
	if(CurrentlyEquippedWeapon && CurrentlyEquippedWeapon->ImpactParticleSystem)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CurrentlyEquippedWeapon->ImpactParticleSystem, Loc + SurfaceNormal*0.01f, SurfaceNormal.Rotation());
		if(NiagaraComponent)
		{
			NiagaraComponent->SetFloatParameter(FName("ImpactScale"), 0.3f);	
		}
	}
}

void ADefaultPlayerCharacter::Multi_SpawnBulletParticles_Implementation(FVector StartLoc, FVector EndLoc)
{
	if(NiagaraSystem)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, StartLoc, UKismetMathLibrary::FindLookAtRotation(StartLoc, EndLoc));	
	}
}

void ADefaultPlayerCharacter::Server_EndShooting_Implementation()
{
	EndShooting();
	
}

bool ADefaultPlayerCharacter::IsInPlantZone() const
{
	return bIsInPlantZone;
}

bool ADefaultPlayerCharacter::HasBomb() const
{
	return bHasBomb;
}

bool ADefaultPlayerCharacter::IsPlantAllowed() const
{
	return bAllowPlant;
}

void ADefaultPlayerCharacter::SetIsInPlantZone(bool bIs)
{
	if(HasAuthority())
	{
		this->bIsInPlantZone = bIs;
		CheckPlantRequirements();
	}
}

void ADefaultPlayerCharacter::SetHasBomb(bool bHas)
{
	if(HasAuthority())
	{
		this->bHasBomb = bHas;
		OnRep_HasBomb();
		CheckPlantRequirements();
	}
}

void ADefaultPlayerCharacter::CheckPlantRequirements()
{
	if(HasAuthority())
	{
		bAllowPlant = HasBomb() && IsInPlantZone();
		if(AStandardCombatGameState* CombatGameState = Cast<AStandardCombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
		{
			bAllowPlant &= CombatGameState->GetCurrentGamePhase().GamePhase == EGamePhase::ActiveGame;
			//cancel plant if somehow it became false;
			if(!bAllowPlant && CombatGameState->IsSomeonePlanting())
			{
				CombatGameState->SetPlayerPlanting(this, false);
			}
		}
		OnRep_AllowPlant();
	}
}

void ADefaultPlayerCharacter::TryStartPlanting()
{
	if(HasAuthority())
	{
		CheckPlantRequirements();
		if(IsPlantAllowed())
		{
			if(AStandardCombatGameMode* CombatGameMode = Cast<AStandardCombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				CancelReload();
				CombatGameMode->BeginPlanting(this);
			}
		}
		
		
	}else
	{
		Server_TryStartPlanting();
	}
}

void ADefaultPlayerCharacter::Server_TryStartPlanting_Implementation()
{
	TryStartPlanting();
}

void ADefaultPlayerCharacter::TryStopPlanting()
{
	if(HasAuthority())
	{
		
		if(AStandardCombatGameMode* CombatGameMode = Cast<AStandardCombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			CombatGameMode->EndPlanting(this);
		}
		
	}else
	{
		Server_TryStopPlanting();
	}
}

void ADefaultPlayerCharacter::Server_TryStopPlanting_Implementation()
{
	TryStopPlanting();
}
