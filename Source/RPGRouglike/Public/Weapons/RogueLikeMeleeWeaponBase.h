// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/RogueLikeWeaponBase.h"
#include "RogueLikeMeleeWeaponBase.generated.h"

class UBoxComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class URogueLikeWeaponFXComponent;

/**
 *
 */
UCLASS()
class RPGROUGLIKE_API ARogueLikeMeleeWeaponBase : public ARogueLikeWeaponBase
{
    GENERATED_BODY()
public:
    ARogueLikeMeleeWeaponBase();
    virtual void Attack(FAttackData InitAttackData) override;

protected:
    virtual void BeginPlay() override;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UBoxComponent* CollisionMesh;

    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
    URogueLikeWeaponFXComponent* FXComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FName WeaponSocketName = "WeaponSocket";
    FTimerHandle FMaterialTimerHandle;

private:
    void ToDefaultMaterial();

    UFUNCTION()
    void OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    // UPROPERTY()
    // UNiagaraComponent* TraceFXComponent;
    // void InitTraceFX();
    // void SetTraceFXVisibility(bool bVisible);

    AController* GetController() const;
    // UNiagaraComponent* SpawnTraceFX();
};
