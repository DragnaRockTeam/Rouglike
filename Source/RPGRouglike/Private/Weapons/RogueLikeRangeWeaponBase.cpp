// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/RogueLikeRangeWeaponBase.h"
#include "Weapons/RogueLikeProjectileBase.h"
#include "Character/RoguelikeCharacter.h"
#include "Kismet/GameplayStatics.h"

void ARogueLikeRangeWeaponBase::Attack(FAttackData InitAttackData)
{
    Super::Attack(InitAttackData);
    MakeShot();
}

bool ARogueLikeRangeWeaponBase::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    if (!GetWorld()) return false;
    auto* Character = Cast<ARogueLikeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!Character) return false;

    TraceStart = GetMuzzleWorldLocation();
    const FVector CursorPosition = Character->GetCursorImpactPoint();

    const FVector ShootDirection = (CursorPosition - TraceStart).GetSafeNormal2D();
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

void ARogueLikeRangeWeaponBase::MakeHit(FHitResult& HitResult, FVector& TraceStart, FVector& TraceEnd)
{
    if (!GetWorld()) return;

    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(GetOwner());
    CollisionParams.bReturnPhysicalMaterial = true;
    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
}
