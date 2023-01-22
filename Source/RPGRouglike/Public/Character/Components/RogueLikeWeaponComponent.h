// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RogueLikeCoreTypes.h"
#include "RogueLikeWeaponComponent.generated.h"

class ARogueLikeWeaponBase;
class ARogueLikeCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPGROUGLIKE_API URogueLikeWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URogueLikeWeaponComponent();
    void StartAttack();
    void StopAttack();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponGripSocketName = "GripSocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    UDataTable* WeaponDataTable;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    UDataTable* WeaponEffectAttributeDataTable;

private:
    FWeaponCharacteristic* WeaponCharacteristic;
    FWeaponData* WeaponData;
    // Can be only > 1
    int32 WeaponLevel = 1;

    ARogueLikeCharacter* Character;
    bool bIsAnimOnProgress = false;
    bool bIsNextAttackCritical = false;

    int32 CurrentAttackAnimationIndex = 0;
    ARogueLikeWeaponBase* Weapon;

    FAttackData GetAttackData();
    FEffectData GetAttackEffectData();
    void SpawnWeapon();
    void AttachWeaponToSocket(ARogueLikeWeaponBase* WeaponToAttach, USceneComponent* OwnerMesh, const FName& SocketName);

    void PlayAnimMontage(UAnimMontage* Animation);
    void InitAttackLoop();
    void NextAnimationIndex();

    UFUNCTION()
    void OnAttackFinished(USkeletalMeshComponent* MeshComponent);

    bool CanAttack() const;
    void Attack();
};
