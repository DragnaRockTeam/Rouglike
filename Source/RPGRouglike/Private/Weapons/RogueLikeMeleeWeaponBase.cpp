// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/RogueLikeMeleeWeaponBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Components/RogueLikeWeaponFXComponent.h"

ARogueLikeMeleeWeaponBase::ARogueLikeMeleeWeaponBase()
{
    CollisionMesh = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
    CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
}

void ARogueLikeMeleeWeaponBase::Attack(FAttackData InitAttackData)
{
    Super::Attack(InitAttackData);
    if (AttackData.Material)
    {
        WeaponMesh->SetMaterial(0, AttackData.Material);
        GetWorld()->GetTimerManager().SetTimer(FMaterialTimerHandle, this, &ARogueLikeMeleeWeaponBase::ToDefaultMaterial, 1.f, false, 3.f);
    }
    else
        ToDefaultMaterial();
}
void ARogueLikeMeleeWeaponBase::ToDefaultMaterial()
{
    WeaponMesh->SetMaterial(0, WeaponData.Material);
}
void ARogueLikeMeleeWeaponBase::BeginPlay()
{
    Super::BeginPlay();
}

void ARogueLikeMeleeWeaponBase::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor) return;
    UGameplayStatics::ApplyDamage(OtherActor, AttackData.Damage, GetController(), GetOwner(), AttackData.DamageType);
    FXComponent->PlayImpactFX(SweepResult, AttackData.ImpactFX);
    WeaponMesh->SetMaterial(0, WeaponData.Material);
}
void ARogueLikeMeleeWeaponBase::InitTraceFX()
{
    if (!TraceFXComponent)
    {
        TraceFXComponent = SpawnTraceFX();
    }
    SetTraceFXVisibility(true);
}

void ARogueLikeMeleeWeaponBase::SetTraceFXVisibility(bool bVisible)
{
    if (TraceFXComponent)
    {
        TraceFXComponent->SetPaused(!bVisible);
        TraceFXComponent->SetVisibility(bVisible, true);
    }
}

AController* ARogueLikeMeleeWeaponBase::GetController() const
{
    const auto Pawn = Cast<APawn>(GetOwner());
    return Pawn ? Pawn->GetController() : nullptr;
}

UNiagaraComponent* ARogueLikeMeleeWeaponBase::SpawnTraceFX()
{
    return UNiagaraFunctionLibrary::SpawnSystemAttached(AttackData.TraceFX,  //
        WeaponMesh,                                                          //
        WeaponSocketName,                                                    //
        FVector::ZeroVector,                                                 //
        FRotator::ZeroRotator,                                               //
        EAttachLocation::SnapToTarget,                                       //
        true);
}
