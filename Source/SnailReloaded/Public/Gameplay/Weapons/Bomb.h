// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Bomb.generated.h"

UCLASS()
class SNAILRELOADED_API ABomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABomb();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* BombMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USphereComponent* DeathRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USphereComponent* DefuseRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UHealthComponent* HealthComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	EGameTeams GetBombTeam();

	UPROPERTY(Replicated)
	TArray<AActor*> PlayersInDeathRadius;
	UPROPERTY(Replicated)
	TArray<ADefaultPlayerCharacter*> PlayersInDefuseRadius;

	UFUNCTION()
	void PlayerEnteredDefuse(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void PlayerLeftDefuse(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void PlayerEnteredDeath(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void PlayerLeftDeath(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void PlantBomb();
	UFUNCTION(BlueprintCallable)
	void ExplodeBomb();
	UFUNCTION(BlueprintPure)
	bool IsPlayerInDefuseRadius(ADefaultPlayerCharacter* Player);

};
