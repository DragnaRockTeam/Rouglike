// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueLikeCoreTypes.h"
#include "RogueLikeWeaponBase.generated.h"

UCLASS()
class RPGROUGLIKE_API ARogueLikeWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    ARogueLikeWeaponBase();
    virtual void Attack(FAttackData InitAttackData);

protected:
    virtual void BeginPlay() override;
    FWeaponData WeaponData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    USkeletalMeshComponent* WeaponMesh;

    FAttackData AttackData;

private:
    void Init(FWeaponData InitData) { WeaponData = InitData; }
    friend class URogueLikeWeaponComponent;
};
