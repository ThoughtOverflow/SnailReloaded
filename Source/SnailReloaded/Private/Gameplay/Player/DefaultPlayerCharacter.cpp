// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ThumbnailHelpers.h"
#include "Components/ArmoredHealthComponent.h"
#include "Components/HealthComponent.h"
#include "Engine/DamageEvents.h"
#include "Framework/DefaultGameMode.h"
#include "Framework/DefaultPlayerController.h"
#include "Framework/Combat/CombatGameMode.h"
#include "Framework/Combat/CombatPlayerController.h"
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

	PrimaryWeapon = nullptr;
	SecondaryWeapon = nullptr;
	MeleeWeapon = nullptr;
	LineTraceMaxDistance = 20000.f;
	FiredRoundsPerShootingEvent = 0;
	LastFireTime = 0.f;
	bAllowAutoReload = true;
	
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
		if(Action.GetValue().Get<bool>())
		{
			FDamageRequest DamageRequest;
			DamageRequest.SourceActor = this;
			DamageRequest.DeltaDamage = 15.f;
			DamageRequest.TargetActor = this;
			Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->ChangeObjectHealth(DamageRequest);	
		}
}

void ADefaultPlayerCharacter::HandleFireInput(const FInputActionInstance& Action)
{
	bool ShootingInput = Action.GetValue().Get<bool>();
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

void ADefaultPlayerCharacter::OnReloadComplete()
{
	if(HasAuthority() && CurrentlyEquippedWeapon)
	{
		//Set new ammo amount:
		float ClipAddAmount = FMath::Min(CurrentlyEquippedWeapon->GetMaxClipAmmo() - CurrentlyEquippedWeapon->GetCurrentClipAmmo(), CurrentlyEquippedWeapon->GetCurrentTotalAmmo());
		CurrentlyEquippedWeapon->SetCurrentClipAmmo(CurrentlyEquippedWeapon->GetCurrentClipAmmo() + ClipAddAmount);
		CurrentlyEquippedWeapon->SetCurrentTotalAmmo(CurrentlyEquippedWeapon->GetCurrentTotalAmmo() - ClipAddAmount);
		CurrentlyEquippedWeapon->SetIsReloading(false);
		UE_LOG(LogTemp, Warning, TEXT("Clip Ammo: %d - Total ammo: %d"), CurrentlyEquippedWeapon->GetCurrentClipAmmo(), CurrentlyEquippedWeapon->GetCurrentTotalAmmo());
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

void ADefaultPlayerCharacter::OnHealthChanged(FDamageResponse DamageResponse)
{
	if(ACombatPlayerController* PlayerController = Cast<ACombatPlayerController>(GetController()))
	{
		if(IsLocallyControlled())
		{
			PlayerController->GetHudData()->SetPlayerHealthPercentage(PlayerHealthComponent->GetObjectHealth() / PlayerHealthComponent->GetObjectMaxHealth())->Submit();
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
	EnhancedInputComponent->BindAction(FireInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HandleFireInput);
	EnhancedInputComponent->BindAction(FireInput, ETriggerEvent::Completed, this, &ADefaultPlayerCharacter::HandleFireInput);
	EnhancedInputComponent->BindAction(ReloadInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HandleReloadInput);
	EnhancedInputComponent->BindAction(SelectPrimaryInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HandleSelectPrimaryInput);
	EnhancedInputComponent->BindAction(SelectSecondaryInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HandleSelectSecondaryInput);
	EnhancedInputComponent->BindAction(SelectMeleeInput, ETriggerEvent::Triggered, this, &ADefaultPlayerCharacter::HandleSelectMeleeInput);
	
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
		if(TestWpn2)
		{
			AssignWeapon(TestWpn2);
		}
		if(TestWpn3)
		{
			AssignWeapon(TestWpn3);
		}

		//Load Default hud for player UI
		Client_LoadDefaultHudData();
		
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
		Weapon->SetIsEquipped(false);
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
		if(CanWeaponFireInMode())
		{
			FiredRoundsPerShootingEvent++;
			
			if(GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLoc, TraceEndLoc,ECC_Visibility, QueryParams))
			{
				Multi_SpawnImpactParticles(HitResult.ImpactPoint, HitResult.ImpactNormal);
				if(UHealthComponent* HealthComponent = Cast<UHealthComponent>(HitResult.GetActor()->GetComponentByClass(UHealthComponent::StaticClass())))
				{
					if(!HealthComponent->bIsDead)
					{
						FDamageRequest DamageRequest;
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
			if (CanWeaponFireInMode() && WeaponHasAmmo() && !CurrentlyEquippedWeapon->GetIsReloading())
			{
				//Add to Combo counter
				FiredRoundsPerShootingEvent++;
				CurrentlyEquippedWeapon->SetCurrentClipAmmo(CurrentlyEquippedWeapon->GetCurrentClipAmmo()-1);
				
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

					// UE_LOG(LogTemp, Warning, TEXT("Values: %f, %f"), FMath::RadiansToDegrees(FMath::Atan2(FMath::Abs(RandDeviationX), LineTraceMaxDistance)), FMath::RadiansToDegrees(FMath::Atan2(FMath::Abs(RandDeviationY), LineTraceMaxDistance)));

					TraceEndLoc += EndDeviation;

					Multi_SpawnBulletParticles(TraceStartLoc, TraceEndLoc);
					
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
									FDamageRequest DamageRequest;
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
			}
		}
		else
		{
			TraceEndLoc = TraceStartLoc + GetController()->GetControlRotation().Vector() * LineTraceMaxDistance;
			//Can Shoot:
			if (CanWeaponFireInMode() && WeaponHasAmmo() && !CurrentlyEquippedWeapon->GetIsReloading())
			{
				//Add to Combo counter
				FiredRoundsPerShootingEvent++;
				CurrentlyEquippedWeapon->SetCurrentClipAmmo(CurrentlyEquippedWeapon->GetCurrentClipAmmo() - 1);
				Multi_SpawnBulletParticles(TraceStartLoc, TraceEndLoc);
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
								FDamageRequest DamageRequest;
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
		bool bRequiredTimeElapsed = GetWorld()->TimeSince(LastFireTime) >= CurrentlyEquippedWeapon->MinimumFireDelay;
		//Check if it is a melee weapon - the elapsed time check can be used as a time between melee attacks check as well.
		if(CurrentlyEquippedWeapon->WeaponSlot == EWeaponSlot::Melee)
		{
			return GetWorld()->TimeSince(LastFireTime) >= CurrentlyEquippedWeapon->MinimumFireDelay && FiredRoundsPerShootingEvent<1;
		}
		//If the weapon has already been firing at least one round, check the actual fire rate for correction
		float WeaponFireDelay = 60 / CurrentlyEquippedWeapon->FireRate;
		if(FiredRoundsPerShootingEvent>=1) bRequiredTimeElapsed &= GetWorld()->TimeSince(LastFireTime) >= WeaponFireDelay;
		
		if(bRequiredTimeElapsed) LastFireTime = GetWorld()->TimeSeconds;
		switch (CurrentlyEquippedWeapon->WeaponMode)
		{
		case EWeaponMode::Automatic: return bRequiredTimeElapsed;
		case EWeaponMode::SemiAutomatic: return FiredRoundsPerShootingEvent<1 && bRequiredTimeElapsed;
		case EWeaponMode::Burst: return FiredRoundsPerShootingEvent<CurrentlyEquippedWeapon->BurstAmount && bRequiredTimeElapsed;
		default: return false;
		}
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

void ADefaultPlayerCharacter::Client_LoadDefaultHudData_Implementation()
{
	if(ACombatPlayerController* PlayerController = Cast<ACombatPlayerController>(GetController()))
	{
		if(PlayerController->PlayerHud)
		{
			UHudData* HudData = PlayerController->GetHudData();
			HudData->SetPlayerHealthPercentage(PlayerHealthComponent->GetObjectHealth() / PlayerHealthComponent->GetObjectMaxHealth());
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