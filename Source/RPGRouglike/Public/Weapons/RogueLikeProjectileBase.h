// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueLikeCoreTypes.h"
#include "RogueLikeProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class URogueLikeWeaponFXComponent;

UCLASS()
class RPGROUGLIKE_API ARogueLikeProjectileBase : public AActor
{
    GENERATED_BODY()

public:
    ARogueLikeProjectileBase();

    UFUNCTION(BlueprintCallable)
    void SetShotDirection(const FVector& Direction) { ShotDirection = Direction; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
    UProjectileMovementComponent* MovementComponent;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UStaticMeshComponent* ProjectileMesh;

    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
    URogueLikeWeaponFXComponent* FXComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    FName ProjectileSocketName = "ProjectileSocket";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX", meta = (ClampMin = 0.f, UIMin = 0.0f, ClampMax = 50.f, UIMax = 50.f))
    float FXLifeTime = 1.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float LifeSeconds = 5.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float ProjectileBaseSpeed = 2000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FAttackData ProjectileData;

private:
    FVector ShotDirection;
    void Init(FAttackData InitData) { ProjectileData = InitData; }

    UPROPERTY()
    UNiagaraComponent* TraceFXComponent;

    UFUNCTION()
    void OnProjecttileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);

    void InitTraceFX();
    void SetTraceFXVisibility(bool bVisible);

    AController* GetController() const;
    UNiagaraComponent* SpawnTraceFX();

    friend class ARogueLikeRangeWeaponBase;
};
