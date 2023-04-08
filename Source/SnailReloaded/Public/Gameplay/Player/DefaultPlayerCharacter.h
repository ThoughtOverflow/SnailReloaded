// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "InputMappingContext.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputMappingContext* PlayerMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* MoveInput;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* LookInput;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inputs")
	UInputAction* HP_Test;
	
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

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> TestWpn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> TestWpn2;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> TestWpn3;
	
	
};
