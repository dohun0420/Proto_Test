#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Components/StaticMeshComponent.h"
#include "AutoTurret.generated.h" // 반드시 include 목록 맨 마지막에 배치

UCLASS()
class AAutoTurret : public AActor
{
	GENERATED_BODY()

public:
	AAutoTurret();

protected:
	// ★ 아래 두 함수 선언이 추가되어야 .cpp 파일의 구현부를 클래스 멤버로 인식합니다.
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TurretMesh;

	void UpdateTarget();
	void FireMuzzleRay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FName MuzzleSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Visual")
	float LaserThickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Visual")
	float LaserDuration;

	FTimerHandle FireTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AActor* CurrentTarget;
};