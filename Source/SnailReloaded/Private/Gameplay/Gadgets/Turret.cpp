// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Gadgets/Turret.h"


#include "Components/SphereComponent.h"
#include "Framework/Combat/CombatGameMode.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"

ATurret::ATurret()
{

	GadgetHealthComponent->GameObjectType.ObjectType = EGameObjectTypes::Turret;
	SightRadius = CreateDefaultSubobject<USphereComponent>(TEXT("SightRadius"));
	SightRadius->SetupAttachment(GadgetMesh);
	SightRadius->OnComponentEndOverlap.AddDynamic(this, &ATurret::PlayerExit);
	SightRadius->OnComponentBeginOverlap.AddDynamic(this, &ATurret::PlayerEnter);

}

void ATurret::FireTurret()
{
	if(CurrentTarget && HasAuthority())
	{
		FHitResult HitResult;
		FVector TraceStartLoc = GetActorLocation();
	
		FVector TraceEndLoc = CurrentTarget->GetActorLocation();
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);


		if (GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLoc, TraceEndLoc,
																		   ECC_Visibility, QueryParams))
		{
			//Process hit results:
			if(HitResult.GetActor())
			{
				if(HitResult.GetActor()==CurrentTarget)
				{
					if(UHealthComponent* HealthComponent = Cast<UHealthComponent>(HitResult.GetActor()->GetComponentByClass(UHealthComponent::StaticClass())))
					{
						if(!HealthComponent->bIsDead)
						{
							FDamageRequest DamageRequest = FDamageRequest();
							DamageRequest.SourceActor = this;
							DamageRequest.TargetActor = HitResult.GetActor();
							DamageRequest.DeltaDamage = -10;
							//FMath::RoundToFloat(CurrentlyEquippedWeapon->DamageCurve->GetFloatValue((HitResult.ImpactPoint - TraceStartLoc).Size() / 100.f));
                    				
							Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->ChangeObjectHealth(DamageRequest);
						}
                    							
					}
				}else
				{
					CheckTarget();
				}
				//Multi_SpawnImpactParticles(HitResult.ImpactPoint, HitResult.ImpactNormal);
				// DrawDebugLine(GetWorld(), TraceStartLoc, HitResult.ImpactPoint, FColor::Magenta, true, -1, 0, 3);
				
			}
		
		}
	}
}

void ATurret::InitiateShooting()
{
	GetWorld()->GetTimerManager().ClearTimer(ShootingTimer);
	GetWorld()->GetTimerManager().SetTimer(ShootingTimer, this, &ATurret::FireTurret, 1.f, true);
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	
}

void ATurret::PlayerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	CheckTarget();
	
	
}

void ATurret::PlayerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority())
	{
		int32 PlayerCount = 0;
		TArray<AActor*> Actors;
		SightRadius->GetOverlappingActors(Actors);
		for(AActor* Actor: Actors)
		{
			if(UHealthComponent* HealthComponent= Cast<UHealthComponent>(Actor->GetComponentByClass(UHealthComponent::StaticClass())))
			{
			
				FHitResult HitResult;
				FVector TraceStartLoc = GetActorLocation();
				FVector TraceEndLoc = Actor->GetActorLocation();
				FCollisionQueryParams Params;
				if(GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLoc, TraceEndLoc, ECC_Visibility, Params))
				{
					if(HitResult.GetActor() == Actor)
					{
						PlayerCount++;
					}
				}
			}
		
		}
		if (PlayerCount == 1)
		{
			CheckTarget();
		}	
	}
}

void ATurret::CheckTarget()
{
	if(HasAuthority())
	{
		CurrentTarget = nullptr;
		TArray<AActor*> Actors;
		SightRadius->GetOverlappingActors(Actors);
		for(AActor* Actor: Actors)
		{
			if(UHealthComponent* HealthComponent= Cast<UHealthComponent>(Actor->GetComponentByClass(UHealthComponent::StaticClass())))
			{
				FHitResult HitResult;
				FVector TraceStartLoc = GetActorLocation();
				FVector TraceEndLoc = Actor->GetActorLocation();
				FCollisionQueryParams Params;
				if(GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLoc, TraceEndLoc, ECC_Visibility, Params))
				{
					if(HitResult.GetActor() == Actor)
					{
					
						CurrentTarget = Actor;
						InitiateShooting();
						return;
					}
				}
			}
	}
		
	}
	
	
	/**ADefaultPlayerCharacter* PlayerCharacter= Cast<ADefaultPlayerCharacter>(Actor);
	FHitResult HitResult;
	FVector TraceStartLoc = GetActorLocation();
	FVector TraceEndLoc = PlayerCharacter->GetActorLocation();
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerCharacter);
	if(GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLoc, TraceEndLoc, ECC_Visibility, Params))
	{
		if(HitResult.GetActor() == Actor)
	}*/
}

void ATurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATurret, CurrentTarget);
}

