// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Components/RogueLikeHealthComponent.h"
#include "Character/RogueLikeCharacter.h"

URogueLikeHealthComponent::URogueLikeHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

float URogueLikeHealthComponent::GetHealthPrecent() const
{
    return Owner ? Health / Owner->GetMaxHealth() : 0.0f;
}

void URogueLikeHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    Owner = Cast<ARogueLikeCharacter>(GetOwner());
    check(Owner);

    Owner->OnCharacterDataInit.AddDynamic(this, &ThisClass::OnCharacterDataInit);
    Owner->OnTakeAnyDamage.AddDynamic(this, &ThisClass::OnTakeAnyDamageHandle);
    Owner->OnPassiveCharacteristicUpdated.AddDynamic(this, &ThisClass::UpdateHealRate);
}

void URogueLikeHealthComponent::OnTakeAnyDamageHandle(
    AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* DamageInstigatedBy, AActor* DamageCauser)
{
    if (!Owner || Damage <= 0.0f || IsDead() || !GetWorld() || Owner->GetDamageResistance() > 1.0f) return;
    float RealDamage = Damage - Damage * Owner->GetDamageResistance();

    SetHealth(Health - RealDamage);

    if (IsDead()) OnDeath.Broadcast();

    GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(
        HealTimerHandle, this, &URogueLikeHealthComponent::HealUpdate, Owner->GetHealthRegenerationRate(), true, RegenerationDelay);
}

void URogueLikeHealthComponent::UpdateHealRate()
{
    GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(
        HealTimerHandle, this, &URogueLikeHealthComponent::HealUpdate, Owner->GetHealthRegenerationRate(), true, 0.0f);
}

void URogueLikeHealthComponent::HealUpdate()
{
    SetHealth(Health + HealModifire);
    if (IsHealthFull() && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    }
}

void URogueLikeHealthComponent::SetHealth(float NewHealth)
{
    const auto NextHealth = FMath::Clamp(NewHealth, 0.0f, Owner->GetMaxHealth());
    const auto HealhtDelta = NextHealth - Health;

    Health = NextHealth;
    OnHealthUpdate.Broadcast();
}

void URogueLikeHealthComponent::OnCharacterDataInit()
{
    check(Owner);
    SetHealth(Owner->GetMaxHealth());
}

bool URogueLikeHealthComponent::IsHealthFull() const
{
    return Owner ? FMath::IsNearlyEqual(Health, Owner->GetMaxHealth()) : false;
}
