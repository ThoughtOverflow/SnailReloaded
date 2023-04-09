// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Gameplay/Weapons/WeaponBase.h"
#include "DefaultPlayerCharacter.generated.h"


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
	AWeaponBase* CurrentlyEquippedWeapon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons", Replicated)
	float LineTraceMaxDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons", Replicated)
	int32 FiredRoundsPerShootingEvent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons")
	UNiagaraSystem* NiagaraSystem;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void Move(const FInputActionInstance& Action);
	UFUNCTION()
	void Look(const FInputActionInstance& Action);
	UFUNCTION()
	void HealthChange(const FInputActionInstance& Action);
	UFUNCTION()
	void HandleFireInput(const FInputActionInstance& Action);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Called when the controller possesses this player (When network is ready for RPC calls)
	virtual void OnPlayerPossessed(ADefaultPlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	bool AssignWeapon(TSubclassOf<AWeaponBase> WeaponClass);
	UFUNCTION(Server,Reliable)
	void Server_AssignWeapon(TSubclassOf<AWeaponBase> WeaponClass);
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
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_SpawnBulletParticles(FVector StartLoc, FVector EndLoc);
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> TestWpn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> TestWpn2;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> TestWpn3;
	
	
};
