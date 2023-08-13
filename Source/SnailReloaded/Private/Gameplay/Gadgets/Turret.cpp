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

	bUsePlacementMode = true;
	GadgetHealthComponent->GameObjectType.ObjectType = EGameObjectTypes::Turret;
	SightRadius = CreateDefaultSubobject<USphereComponent>(TEXT("SightRadius"));
	SightRadius->SetupAttachment(GadgetMesh);
	SightRadius->OnComponentEndOverlap.AddDynamic(this, &ATurret::PlayerExit);
	SightRadius->OnComponentBeginOverlap.AddDynamic(this, &ATurret::PlayerEnter);
	TurretPlacementDistance = 300.f;
	ActorRadialPadding = 30.f;
	
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

	if(IsGadgetInitialized())
	{
		CheckTarget();	
	}else if(IsGadgetInPlacementMode())
	{
		UpdatePlacementLocation();
	}
	
	
}

void ATurret::PlayerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	
	CheckTarget();
	
	
}

void ATurret::PlayerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority() && IsGadgetInitialized())
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
					if(HitResult.GetActor() == Actor && GetOwningTeam()!=HealthComponent->GetOwnerTeam())
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
	if(HasAuthority() && IsGadgetInitialized())
	{
		
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
					if(HitResult.GetActor() == Actor && GetOwningTeam()!=HealthComponent->GetOwnerTeam())
					{
						
						if(Actor!=CurrentTarget)
						{
							CurrentTarget = Actor;
							InitiateShooting();
						}
						
						return;
					}
				}
			}
	}
		CurrentTarget = nullptr;
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

void ATurret::UpdatePlacementLocation()
{
	if(ADefaultPlayerCharacter* OwningPlayer = Cast<ADefaultPlayerCharacter>(OwningPlayerState->GetPawn()))
	{
		FHitResult HitResult;
		FVector StartLoc;
		FRotator StartRot;
		FVector EndLoc;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(OwningPlayer);
		OwningPlayer->GetController()->GetActorEyesViewPoint(StartLoc, StartRot);
		StartLoc = OwningPlayer->CameraComponent->GetComponentLocation();
		EndLoc = StartLoc + StartRot.Vector() * TurretPlacementDistance;

		//Second trace - straight down:

		FHitResult HitResult2;
		float ActivePadding = 0.f;
		bool bFirstCast = GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Visibility, Params);
		if(bFirstCast && HitResult.ImpactNormal.Z < 0.5f)
		{
			ActivePadding = ActorRadialPadding;
			Params.AddIgnoredActor(HitResult.GetActor());
		}

		if(GetWorld()->LineTraceSingleByChannel(HitResult2, bFirstCast ? HitResult.ImpactPoint : EndLoc, (bFirstCast ? FVector(HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, -50.f) : FVector(EndLoc.X, EndLoc.Y, -50.f)), ECC_Visibility, Params))
		{
			if(HitResult2.ImpactNormal.Z > 0.5f)
			{
				SetActorLocation(HitResult2.ImpactPoint - StartRot.Vector() * FVector(1.f,1.f,0.f) * ActivePadding);
			}
		}

		SetActorRotation(FRotator(0.f, OwningPlayer->GetControlRotation().Yaw, 0.f));
		
	}
}

void ATurret::OnInitialized()
{
	Super::OnInitialized();
	CheckTarget();
}

void ATurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATurret, CurrentTarget);
}

void ATurret::CancelledPlacementMode(ACombatPlayerState* OwningState)
{
	Super::CancelledPlacementMode(OwningState);
}

void ATurret::EnteredPlacementMode(ACombatPlayerState* OwningState)
{
	Super::EnteredPlacementMode(OwningState);
	// AttachToActor(OwningState->GetPawn(), FAttachmentTransformRules::KeepWorldTransform);
	UE_LOG(LogTemp, Warning, TEXT("Entered placement mode for turret."));
	
}

