// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Components/RogueLikeWeaponComponent.h"
#include "Weapons/RogueLikeWeaponBase.h"
#include "GameFramework/Character.h"
#include "Animations/AnimUtils.h"
#include "Animations/RogueLikeAttackEndAnimNotify.h"
#include "Character/RogueLikeCharacter.h"
#include "RogueLikeGameInstance.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All);

URogueLikeWeaponComponent::URogueLikeWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void URogueLikeWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
    if (!GetWorld()) return;

    Character = Cast<ARogueLikeCharacter>(GetOwner());
    if (!Character) return;
    check(WeaponEffectAttributeDataTable);
    check(WeaponDataTable);

    WeaponData = *WeaponDataTable->FindRow<FWeaponData>(Character->GetWeaponRowName(), TEXT("Weapon Context"));

    const auto GameInst = Cast<URogueLikeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    checkf(GameInst, TEXT("GameInst is not valid. GameInst should be child of URogueLikeGameInstance"));

    WeaponLevel = GameInst->GetWeaponLevel();
    checkf(WeaponLevel > 0 && WeaponLevel - 1 < WeaponData.LevelWeaponCharacteristics.Num(),
        TEXT("The weapon level is not valid current weapon level: %d Current weapon levels: %d"), WeaponLevel,
        WeaponData.LevelWeaponCharacteristics.Num());

    WeaponCharacteristic = WeaponData.LevelWeaponCharacteristics[WeaponLevel - 1];
    SpawnWeapon();
    checkf(WeaponData.Animations.Num() > 0, TEXT("Animations doesn't set, Weapon Row name: %s"),  //
        *Character->GetWeaponRowName().ToString());
}

void URogueLikeWeaponComponent::StartAttack()
{
    if (!CanAttack()) return;
    InitAttackLoop();
    PlayAnimMontage(WeaponData.Animations[CurrentAttackAnimationIndex]);
}

void URogueLikeWeaponComponent::StopAttack()
{
    for (const auto& Animation : WeaponData.Animations)
    {
        auto AttackEnd = AnimUtils::FindNotifyByClass<URogueLikeAttackEndAnimNotify>(Animation);

        if (AttackEnd)
        {
            AttackEnd->OnNotified.RemoveDynamic(this, &URogueLikeWeaponComponent::OnAttackFinished);
        }
        else
        {
            UE_LOG(LogWeaponComponent, Error, TEXT("Attack end anim notify is forgotten to set"));
            checkNoEntry();
        }
    }
    bIsAnimOnProgress = false;
}

void URogueLikeWeaponComponent::Attack()
{
    if (!Weapon) return;

    Weapon->Attack(GetAttackData());
    bIsNextAttackCritical = false;
    PlayAnimMontage(WeaponData.Animations[CurrentAttackAnimationIndex]);
}

FAttackData URogueLikeWeaponComponent::GetAttackData()
{
    check(&WeaponData);

    FAttackData AttackData;
    FEffectData EffectData = GetAttackEffectData();

    FName RowName = *((WeaponData.WeaponBaseName).ToString() + (EffectData.EffectName).ToString());
    FWeaponEffectAttribute* EffectAttribute =
        WeaponEffectAttributeDataTable->FindRow<FWeaponEffectAttribute>(RowName, TEXT("Effect Context"));

    if (!EffectAttribute)
    {
        UE_LOG(LogWeaponComponent, Error, TEXT("Can't find effect attribute by row name: %s"), *RowName.ToString());
        return FAttackData();
    }

    AttackData.Damage = EffectAttribute->DamageMultiplier * (WeaponCharacteristic.BaseDamage + Character->GetDamageAmount()) *
                        (bIsNextAttackCritical && EffectAttribute->CanBeCritical ? 1.5f : 1.0f);
    AttackData.DamageType = EffectData.DamageType;
    AttackData.ImpactFX = EffectAttribute->ImpactFX;
    AttackData.Material = EffectAttribute->ProjectileMaterial;
    AttackData.Mesh = EffectAttribute->Mesh;
    AttackData.Speed = EffectAttribute->SpeedMultiplier * WeaponCharacteristic.SpeedMultiplier * Character->GetProjectileSpeed();
    AttackData.TraceFX = EffectAttribute->TraceFX;
    CurrentTrailMaterial = EffectAttribute->TrailMaterial;

    UGameplayStatics::PlaySound2D(GetWorld(), EffectAttribute->Audio);
    return AttackData;
}

FEffectData URogueLikeWeaponComponent::GetAttackEffectData()
{
    TMap<EAttackEffectChance, int32> EffectChances;
    for (const auto& EffectPair : Character->GetEffectChances())
    {
        EffectChances.Add(EffectPair.Key, WeaponCharacteristic.EffectsChanses[EffectPair.Key] + EffectPair.Value);
    }
    bIsNextAttackCritical = (EffectChances[EAttackEffectChance::Critical] >= FMath::RandRange(0, 100));
    EffectChances.Remove(EAttackEffectChance::Critical);

    for (const auto& EffectPair : EffectChances)
    {
        if (EffectPair.Value > FMath::RandRange(0, 100))
            return FEffectData(EffectsNames[EffectPair.Key], EffectsDamageTypes[EffectPair.Key]);
    }

    return FEffectData("_Usual", UDamageType::StaticClass());
}

void URogueLikeWeaponComponent::SpawnWeapon()
{
    if (!GetWorld()) return;

    Weapon = GetWorld()->SpawnActorDeferred<ARogueLikeWeaponBase>(WeaponData.WeaponClass, FTransform(), GetOwner());
    check(Weapon);
    Weapon->Init(WeaponData);
    Weapon->FinishSpawning(FTransform());

    AttachWeaponToSocket(Weapon, Character->GetMesh(), WeaponGripSocketName);
}

void URogueLikeWeaponComponent::AttachWeaponToSocket(
    ARogueLikeWeaponBase* WeaponToAttach, USceneComponent* OwnerMesh, const FName& SocketName)
{
    if (!WeaponToAttach || !OwnerMesh) return;
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    WeaponToAttach->AttachToComponent(OwnerMesh, AttachmentRules, SocketName);
}

void URogueLikeWeaponComponent::PlayAnimMontage(UAnimMontage* Animation)
{
    if (Character) Character->PlayAnimMontage(Animation, Character->GetAttackSpeed() * WeaponCharacteristic.SpeedMultiplier);
}

void URogueLikeWeaponComponent::InitAttackLoop()
{
    for (const auto& Animation : WeaponData.Animations)
    {
        auto AttackEnd = AnimUtils::FindNotifyByClass<URogueLikeAttackEndAnimNotify>(Animation);

        if (AttackEnd)
        {
            AttackEnd->OnNotified.AddDynamic(this, &URogueLikeWeaponComponent::OnAttackFinished);
        }
        else
        {
            UE_LOG(LogWeaponComponent, Error, TEXT("Attack end anim notify is forgotten to set"));
            checkNoEntry();
        }
    }
}

void URogueLikeWeaponComponent::OnAttackFinished(USkeletalMeshComponent* MeshComponent)
{
    bIsAnimOnProgress = false;
    Attack();
}

bool URogueLikeWeaponComponent::CanAttack() const
{
    return Weapon && !bIsAnimOnProgress;
}

void URogueLikeWeaponComponent::NextAnimationIndex()
{
    CurrentAttackAnimationIndex = (CurrentAttackAnimationIndex + 1) % WeaponData.Animations.Num();
}
