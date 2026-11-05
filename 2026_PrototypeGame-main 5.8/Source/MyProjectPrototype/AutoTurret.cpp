#include "AutoTurret.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AAutoTurret::AAutoTurret()
{
	PrimaryActorTick.bCanEverTick = true;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	RootComponent = TurretMesh;

	DamageAmount = 10.0f;
	FireRate = 0.15f;
	AttackRange = 10000.f;
	MuzzleSocketName = TEXT("Muzzle");

	LaserThickness = 6.0f;
	LaserDuration = 0.15f;
}

void AAutoTurret::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAutoTurret::FireMuzzleRay, FireRate, true);
}

void AAutoTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTarget();

	if (IsValid(CurrentTarget))
	{
		FVector MuzzlePos = TurretMesh->DoesSocketExist(MuzzleSocketName)
			? TurretMesh->GetSocketLocation(MuzzleSocketName)
			: GetActorLocation();

		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(MuzzlePos, CurrentTarget->GetActorLocation());
		FRotator NewRotation = FMath::RInterpTo(TurretMesh->GetComponentRotation(), TargetRotation, DeltaTime, 12.0f);
		TurretMesh->SetWorldRotation(NewRotation);
	}
}

void AAutoTurret::UpdateTarget()
{
	TArray<AActor*> FoundMonsters;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Monster"), FoundMonsters);

	FoundMonsters.RemoveAll([](AActor* Actor) {
		return !IsValid(Actor) || Actor->IsActorBeingDestroyed();
		});

	AActor* ClosestMonster = nullptr;
	float MinDistanceSq = FMath::Square(AttackRange);
	const FVector TurretLocation = GetActorLocation();

	for (AActor* Monster : FoundMonsters)
	{
		const float DistSq = FVector::DistSquared(TurretLocation, Monster->GetActorLocation());
		if (DistSq < MinDistanceSq)
		{
			MinDistanceSq = DistSq;
			ClosestMonster = Monster;
		}
	}

	CurrentTarget = ClosestMonster;
}

void AAutoTurret::FireMuzzleRay()
{
	if (!IsValid(CurrentTarget)) return;

	FVector StartLoc = TurretMesh->DoesSocketExist(MuzzleSocketName)
		? TurretMesh->GetSocketLocation(MuzzleSocketName)
		: GetActorLocation();

	FVector TargetLoc = CurrentTarget->GetActorLocation();
	FVector LaunchDirection = (TargetLoc - StartLoc).GetSafeNormal();
	FVector EndLoc = StartLoc + (LaunchDirection * AttackRange);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Visibility, CollisionParams);

	FVector LaserEndPos = bHit ? HitResult.ImpactPoint : EndLoc;

	DrawDebugLine(GetWorld(), StartLoc, LaserEndPos, FColor::Red, false, LaserDuration, 0, LaserThickness);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor))
		{
			if (HitActor->ActorHasTag("Monster") || HitActor == CurrentTarget)
			{
				UGameplayStatics::ApplyDamage(HitActor, DamageAmount, GetInstigatorController(), this, UDamageType::StaticClass());
			}
		}
	}
}