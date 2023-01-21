// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RogueLikeWeaponFXComponent.generated.h"

class UNiagaraSystem;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPGROUGLIKE_API URogueLikeWeaponFXComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URogueLikeWeaponFXComponent();

    void PlayImpactFX(const FHitResult& HitInfo, UNiagaraSystem* NiagaraEffect);
};
