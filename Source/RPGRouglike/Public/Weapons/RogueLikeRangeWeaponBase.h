// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/RogueLikeWeaponBase.h"
#include "RogueLikeCoreTypes.h"
#include "RogueLikeRangeWeaponBase.generated.h"

/**
 *
 */
UCLASS()
class RPGROUGLIKE_API ARogueLikeRangeWeaponBase : public ARogueLikeWeaponBase
{
    GENERATED_BODY()
public:
    virtual void Attack(FAttackData InitAttackData) override;

protected:
    virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;
    virtual void MakeShot();
    FVector GetMuzzleWorldLocation() const;
    APlayerController* GetPlayerController() const;
    bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;
    void MakeHit(FHitResult& HitResult, FVector& TraceStart, FVector& TraceEnd);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName MuzzleSocketName = "MuzzleSocket";
};
