// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/RogueLikeWeaponFXComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values for this component's properties
URogueLikeWeaponFXComponent::URogueLikeWeaponFXComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URogueLikeWeaponFXComponent::PlayImpactFX(const FHitResult& HitInfo, UNiagaraSystem* NiagaraEffect)
{
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),  //
        NiagaraEffect,                               //
        HitInfo.ImpactPoint,                                    //
        HitInfo.ImpactNormal.Rotation());
}