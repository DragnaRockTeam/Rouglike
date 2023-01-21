// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/RogueLikeWeaponBase.h"

ARogueLikeWeaponBase::ARogueLikeWeaponBase()
{
    PrimaryActorTick.bCanEverTick = true;
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
}

void ARogueLikeWeaponBase::Attack(FAttackData InitAttackData)
{
    AttackData = InitAttackData;
}

void ARogueLikeWeaponBase::BeginPlay()
{
    Super::BeginPlay();
    WeaponMesh->SetSkeletalMesh(WeaponData.WeaponMesh);
    WeaponMesh->SetMaterial(0, WeaponData.Material);
}
