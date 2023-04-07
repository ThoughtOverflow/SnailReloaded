// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "Gameplay/Weapons/WeaponBase.h"
#include "DefaultPlayerCharacter.generated.h"


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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons")
	AWeaponBase* PrimaryWeapon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons")
	AWeaponBase* SecondaryWeapon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons")
	AWeaponBase* MeleeWeapon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapons")
	AWeaponBase* CurrentlyEquippedWeapon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

	UFUNCTION(BlueprintCallable)
	bool AssignWeapon(TSubclassOf<AWeaponBase> WeaponClass);
	UFUNCTION(BlueprintCallable)
	AWeaponBase* EquipWeapon(EWeaponSlot Slot);
	UFUNCTION(BlueprintCallable)
	void UnequipWeapon();
	UFUNCTION(BlueprintPure)
	AWeaponBase* GetWeaponAtSlot(EWeaponSlot Slot);

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> TestWpn;
	
	
};
