// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Gadgets/Turret.h"


#include "Components/SphereComponent.h"
#include "Framework/Combat/CombatGameMode.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HealthComponent.h"

ATurret::ATurret()
{

	SightRadius = CreateDefaultSubobject<USphereComponent>(TEXT("SightRadius"));
	SightRadius->SetupAttachment(GadgetMesh);

}

void ATurret::FireTurret(ADefaultPlayerCharacter* Target)
{
	FHitResult HitResult;
	FVector TraceStartLoc = GetActorLocation();
	
	FVector TraceEndLoc = Target->GetActorLocation();
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);


	if (GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLoc, TraceEndLoc,
																	   ECC_Visibility, QueryParams))
	{
		//Process hit results:
		if(HitResult.GetActor())
		{
			//Multi_SpawnImpactParticles(HitResult.ImpactPoint, HitResult.ImpactNormal);
			// DrawDebugLine(GetWorld(), TraceStartLoc, HitResult.ImpactPoint, FColor::Magenta, true, -1, 0, 3);
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
		}
	}
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> Targets;
	SightRadius->GetOverlappingActors(Targets);
	for(AActor* T : Targets)
	{
		if(ADefaultPlayerCharacter* PotentialT = Cast<ADefaultPlayerCharacter>(T))
		{
			CurrentTarget = PotentialT;
			FireTurret(CurrentTarget);
		}
	}
	
}

