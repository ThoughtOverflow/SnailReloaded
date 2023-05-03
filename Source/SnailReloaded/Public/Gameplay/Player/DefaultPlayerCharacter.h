// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/ArmoredHealthComponent.h"
#include "Gameplay/Weapons/WeaponBase.h"
#include "DefaultPlayerCharacter.generated.h"


struct FShieldProperties;
struct FDamageResponse;
class ACombatPlayerController;
class ADefaultPlayerController;
class UArmoredHealthComponent;
class UHealthComponent;
UCLASS()
class SNAILRELOADED_API ADefaultPlayerCharacter : public ACharacter
{
	
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADefaultPlayerCharacter();

	//InputActions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputMappingContext* PlayerMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* MoveInput;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* LookInput;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* HP_Test;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* FireInput;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* ReloadInput;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* SelectPrimaryInput;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* SelectSecondaryInput;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* SelectMeleeInput;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* ToggleBuyMenu;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* PlantBomb;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCameraComponent* CameraComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UArmoredHealthComponent* PlayerHealthComponent;

	//Weapon system;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons", Replicated)
	AWeaponBase* PrimaryWeapon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons", Replicated)
	AWeaponBase* SecondaryWeapon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons", Replicated)
	AWeaponBase* MeleeWeapon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons", Replicated)
	float LineTraceMaxDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons", Replicated)
	int32 FiredRoundsPerShootingEvent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons", Replicated)
	bool bAllowAutoReload;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons")
	UNiagaraSystem* NiagaraSystem;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Health Component:

	UFUNCTION()
	EGameTeams QueryGameTeam();

	//Helpers:

	UFUNCTION(BlueprintCallable)
	ACombatPlayerController* GetCombatPlayerController();
	
	UFUNCTION(Server, Reliable)
	void Server_BlockPlayerInputs(bool bBlock);
	
	UFUNCTION()
	void Move(const FInputActionInstance& Action);
	UFUNCTION()
	void Look(const FInputActionInstance& Action);
	UFUNCTION()
	void HealthChange(const FInputActionInstance& Action);
	UFUNCTION()
	void HandleFireInput(const FInputActionInstance& Action);
	UFUNCTION()
	void HandleReloadInput(const FInputActionInstance& Action);
	UFUNCTION()
	void HandleSelectPrimaryInput(const FInputActionInstance& Action);
	UFUNCTION()
	void HandleSelectSecondaryInput(const FInputActionInstance& Action);
	UFUNCTION()
	void HandleSelectMeleeInput(const FInputActionInstance& Action);
	UFUNCTION()
	void HandleToggleBuyMenu(const FInputActionInstance& Action);
	UFUNCTION()
	void HandlePlantBomb(const FInputActionInstance& Action);
	
	//Shooting
	
	UPROPERTY()
	float LastFireTime;
	UPROPERTY()
	FTimerHandle ReloadTimerHandle;

	UFUNCTION()
	void OnReloadComplete();
	UFUNCTION()
	void StartReload();
	UFUNCTION(Server, Reliable)
	void Server_StartReload();
	UFUNCTION(Client, Reliable)
	void Client_StartReload();
	UFUNCTION()
	void CancelReload();
	UFUNCTION()
	void OnRep_CurrentWeapon();
	UFUNCTION()
	void OnHealthChanged(FDamageResponse DamageResponse);
	UFUNCTION()
	void OnShieldHealthChanged();
	UFUNCTION()
	void OnCurrentWeaponAmmoChanged();
	UFUNCTION()
	void OnCurrentWeaponReloading();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons", ReplicatedUsing=OnRep_CurrentWeapon)
	AWeaponBase* CurrentlyEquippedWeapon;

	//Plant logic:
	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bIsInPlantZone;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_HasBomb)
	bool bHasBomb;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_AllowPlant)
	bool bAllowPlant;

	UFUNCTION()
	void OnRep_AllowPlant();
	UFUNCTION()
	void OnRep_HasBomb();
	

	UFUNCTION()
	void TryStartPlanting();
	UFUNCTION(Server, Reliable)
	void Server_TryStartPlanting();
	UFUNCTION()
	void TryStopPlanting();
	UFUNCTION(Server, Reliable)
	void Server_TryStopPlanting();


	UFUNCTION(Server, Reliable)
	void Server_TemporaryShit();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Called when the controller possesses this player (When network is ready for RPC calls)
	virtual void OnPlayerPossessed(ACombatPlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	AWeaponBase* AssignWeapon(TSubclassOf<AWeaponBase> WeaponClass);
	UFUNCTION(Server,Reliable)
	void Server_AssignWeapon(TSubclassOf<AWeaponBase> WeaponClass);
	UFUNCTION(BlueprintCallable)
	bool RemoveWeapon(EWeaponSlot Slot);
	UFUNCTION(Server,Reliable)
	void Server_RemoveWeapon(EWeaponSlot Slot);
	UFUNCTION(BlueprintCallable)
	AWeaponBase* EquipWeapon(EWeaponSlot Slot);
	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(EWeaponSlot Slot);
	UFUNCTION(BlueprintCallable)
	void UnequipWeapon();
	UFUNCTION(Server, Reliable)
	void Server_UnequipWeapon();
	UFUNCTION(BlueprintPure)
	AWeaponBase* GetWeaponAtSlot(EWeaponSlot Slot);

	//Input:
	
	UFUNCTION(BlueprintCallable)
	void BlockPlayerInputs(bool bBlock);

	//Shooting

	UFUNCTION(BlueprintCallable)
	void BeginShooting();
	UFUNCTION(Server, Reliable)
	void Server_BeginShooting();
	UFUNCTION(BlueprintCallable)
	void EndShooting();
	UFUNCTION(Server, Reliable)
	void Server_EndShooting();
	UFUNCTION(BlueprintCallable)
	void UseMeleeWeapon();
	UFUNCTION(BlueprintCallable)
	void FireEquippedWeapon();
	UFUNCTION(BlueprintPure)
	bool CanWeaponFireInMode();
	UFUNCTION(BlueprintPure)
	bool WeaponHasAmmo();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SpawnBulletParticles(FVector StartLoc, FVector EndLoc);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SpawnImpactParticles(FVector Loc, FVector SurfaceNormal);

	UFUNCTION(BlueprintPure)
	AWeaponBase* GetCurrentlyEquippedWeapon();
	UFUNCTION(BlueprintCallable)
	void SetCurrentlyEqippedWeapon(AWeaponBase* NewWeapon);

	//HUD:

	UFUNCTION(Client, Reliable)
	void Client_LoadDefaultHudData();

	UFUNCTION(BlueprintPure)
	float GetReloadProgress();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> TestWpn;

	//Buy system

	UFUNCTION(BlueprintCallable)
	AWeaponBase* EquipStrongestWeapon();
	UFUNCTION(BlueprintCallable)
	void TryPurchaseItem(EItemIdentifier ItemIdentifier);
	UFUNCTION(Server, Reliable)
	void Server_TryPurchaseItem(EItemIdentifier ItemIdentifier);
	UFUNCTION(BlueprintCallable)
	void TrySellItem(EItemIdentifier ItemIdentifier);
	UFUNCTION(Server, Reliable)
	void Server_TrySellItem(EItemIdentifier ItemIdentifier);
	UFUNCTION()
	void UpdateShieldProperties(FShieldProperties ShieldProperties);
	UFUNCTION()
	void StoreCurrentShieldData();
	UFUNCTION(BlueprintGetter)
	bool HasStoredShield();
	UFUNCTION(BlueprintPure)
	TArray<EItemIdentifier> GetAllItems();
	UFUNCTION(BlueprintPure)
	EWeaponSlot GetWeaponSlotByIdentifier(EItemIdentifier Identifier);
	UFUNCTION(BlueprintPure)
	EItemIdentifier GetCurrentShieldType();
	UFUNCTION(BlueprintCallable)
	void RevertToPreviousShield();
	UFUNCTION(BlueprintPure)
	bool CanSellCurrentShield();
	UFUNCTION(BlueprintCallable)
	void SetCanSellCurrentShield(bool bSell);
	UFUNCTION()
	void CheckPlantRequirements();
	UFUNCTION(BlueprintPure)
	bool IsInPlantZone() const;
	UFUNCTION(BlueprintCallable)
	void SetIsInPlantZone(bool bIn);
	UFUNCTION(BlueprintPure)
	bool HasBomb() const;
	UFUNCTION(BlueprintCallable)
	void SetHasBomb(bool bHas);
	UFUNCTION(BlueprintPure)
	bool IsPlantAllowed() const;
	
	
};

