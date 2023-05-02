// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class ACombatPlayerController;

UENUM(BlueprintType)
enum class GameTeams : uint8
{
	None = 0,
	TeamA = 1,
	TeamB = 2
};
UENUM(BlueprintType)
enum class GameObjectTypes : uint8
{
	None = 0,
	AllyPlayer = 1,
	EnemyPlayer = 2,
	Bomb = 3,
		
};

USTRUCT(BlueprintType)
struct FDamageRequest
{
	GENERATED_BODY()

public:
	
	FDamageRequest();

	static FDamageRequest Initialize(UHealthComponent* HealthComponent);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SourceActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeltaDamage;
	
	
};

USTRUCT(BlueprintType)
struct FDamageResponse
{
	GENERATED_BODY()

public:
	
	FDamageResponse();
	FDamageResponse(AActor* SrcActor, float DeltaHealth, float NewHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SourceActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeltaHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NewHealth;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectHealthChanged, FDamageResponse, DamageResponse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectKilled, FDamageResponse, LatestDamage);

UCLASS( ClassGroup=(HealthSystem), meta=(BlueprintSpawnableComponent) )
class SNAILRELOADED_API UHealthComponent : public UActorComponent
{
public:
	

private:
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	float DefaultObjectHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	bool bInvulnerable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated)
	FDamageResponse LatestDamage;

	UPROPERTY(BlueprintAssignable)
	FObjectHealthChanged ObjectHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FObjectKilled ObjectKilled;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_Dead)
	bool bIsDead;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
	UPROPERTY(ReplicatedUsing = OnRep_ObjectHealth)
	float ObjectHealth;
	UPROPERTY(Replicated)
	float ObjectMaxHealth;
	UPROPERTY(EditDefaultsOnly)
	GameObjectTypes ObjectType;
	UPROPERTY(EditDefaultsOnly)
	TMap<GameObjectTypes, float> DamageMultipliers;

	UFUNCTION()
	void OnRep_ObjectHealth();
	UFUNCTION()
	void OnRep_Dead();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
	virtual float GetObjectHealth() const;
	UFUNCTION(BlueprintCallable)
	virtual FDamageResponse ChangeObjectHealth(FDamageRequest DamageRequest);
	virtual FDamageResponse SetObjectHealth(FDamageRequest DamageRequest, float newHealth);
	UFUNCTION(BlueprintPure)
	virtual float GetObjectMaxHealth() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetObjectMaxHealth(float newHealth);
	UFUNCTION(BlueprintPure)
	float GetDamageMultiplierForTarget(UHealthComponent* TargetComp);
	UFUNCTION(BlueprintPure)
	GameObjectTypes GetCurrentObjectType();
		
};
