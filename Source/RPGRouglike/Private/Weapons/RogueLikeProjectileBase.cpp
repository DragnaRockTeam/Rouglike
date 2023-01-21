// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/RogueLikeProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Components/RogueLikeWeaponFXComponent.h"

ARogueLikeProjectileBase::ARogueLikeProjectileBase()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
    FXComponent = CreateDefaultSubobject<URogueLikeWeaponFXComponent>("FXComponent");
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("Projectile");

    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    CollisionComponent->bReturnMaterialOnMove = true;

    CollisionComponent->InitSphereRadius(5.f);
    SetRootComponent(CollisionComponent);
    ProjectileMesh->SetupAttachment(CollisionComponent);

    MovementComponent->ProjectileGravityScale = 0.0f;
}

void ARogueLikeProjectileBase::BeginPlay()
{
    Super::BeginPlay();
    ProjectileMesh->SetStaticMesh(ProjectileData.Mesh);
    ProjectileMesh->SetMaterial(0,ProjectileData.Material);

    MovementComponent->InitialSpeed = ProjectileBaseSpeed * ProjectileData.Speed;
    check(MovementComponent);
    check(FXComponent);

    MovementComponent->Velocity = ShotDirection * MovementComponent->InitialSpeed;
    CollisionComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnProjecttileHit);
    CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
    SetLifeSpan(LifeSeconds);
    InitTraceFX();
}

void ARogueLikeProjectileBase::OnProjecttileHit(
    UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!GetWorld()) return;

    MovementComponent->StopMovementImmediately();
    UGameplayStatics::ApplyDamage(OtherActor, ProjectileData.Damage, GetController(), GetOwner(), ProjectileData.DamageType);

    FXComponent->PlayImpactFX(Hit, ProjectileData.ImpactFX);
    ProjectileMesh->SetVisibility(false, false);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(FXLifeTime);
}
void ARogueLikeProjectileBase::InitTraceFX()
{
    if (!TraceFXComponent)
    {
        TraceFXComponent = SpawnTraceFX();
    }
    SetTraceFXVisibility(true);
}

void ARogueLikeProjectileBase::SetTraceFXVisibility(bool bVisible)
{
    if (TraceFXComponent)
    {
        TraceFXComponent->SetPaused(!bVisible);
        TraceFXComponent->SetVisibility(bVisible, true);
    }
}

AController* ARogueLikeProjectileBase::GetController() const
{
    const auto Pawn = Cast<APawn>(GetOwner());
    return Pawn ? Pawn->GetController() : nullptr;
}

UNiagaraComponent* ARogueLikeProjectileBase::SpawnTraceFX()
{
    return UNiagaraFunctionLibrary::SpawnSystemAttached(ProjectileData.TraceFX,  //
        ProjectileMesh,                                                                    //
        ProjectileSocketName,                                                              //
        FVector::ZeroVector,                                                               //
        FRotator::ZeroRotator,                                                             //
        EAttachLocation::SnapToTarget,                                                     //
        true);
}
