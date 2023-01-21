// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/RogueLikeRangeWeaponBase.h"
#include "Weapons/RogueLikeProjectileBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"

void ARogueLikeRangeWeaponBase::Attack(FAttackData InitAttackData)
{
    Super::Attack(InitAttackData);
    MakeShot();
}

bool ARogueLikeRangeWeaponBase::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;
    if (!GetPlayerViewPoint(ViewLocation, ViewRotation)) return false;

    TraceStart = ViewLocation;
    const FVector ShootDirection = ViewRotation.Vector();
    TraceEnd = TraceStart + ShootDirection * 1000.f;
    return true;
}

void ARogueLikeRangeWeaponBase::MakeShot()
{
    if (!GetWorld()) return;

    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd)) return;

    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);

    const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
    const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();

    const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
    ARogueLikeProjectileBase* ProjectTile =
        GetWorld()->SpawnActorDeferred<ARogueLikeProjectileBase>(ARogueLikeProjectileBase::StaticClass(), SpawnTransform);
    if (!ProjectTile) return;

    ProjectTile->SetOwner(GetOwner());
    ProjectTile->SetShotDirection(Direction);
    ProjectTile->Init(AttackData);
    ProjectTile->FinishSpawning(SpawnTransform);
}

FVector ARogueLikeRangeWeaponBase::GetMuzzleWorldLocation() const
{
    return WeaponMesh->GetSocketLocation(MuzzleSocketName);
}

APlayerController* ARogueLikeRangeWeaponBase::GetPlayerController() const
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player) return nullptr;

    return Player->GetController<APlayerController>();
}

bool ARogueLikeRangeWeaponBase::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const
{
    const auto Controller = GetPlayerController();
    if (!Controller) return false;

    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    return true;
}

void ARogueLikeRangeWeaponBase::MakeHit(FHitResult& HitResult, FVector& TraceStart, FVector& TraceEnd)
{
    if (!GetWorld()) return;

    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(GetOwner());
    CollisionParams.bReturnPhysicalMaterial = true;
    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
}
