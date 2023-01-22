// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Components/RogueLikeStaminaComponent.h"
#include "Character/Components/RoguelikeHealthComponent.h"
#include "Character/RogueLikeCharacter.h"

URogueLikeStaminaComponent::URogueLikeStaminaComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

float URogueLikeStaminaComponent::GetStaminaPrecent() const
{
    return Owner ? Stamina / Owner->GetMaxStamina() : 0.0f;
}

bool URogueLikeStaminaComponent::IsStaminaFull() const
{
    return Owner ? FMath::IsNearlyEqual(Stamina, Owner->GetMaxStamina()) : false;
}

void URogueLikeStaminaComponent::StartRunning()
{
    GetWorld()->GetTimerManager().ClearTimer(RegenerationTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(
        ReduceTimerHandle, this, &URogueLikeStaminaComponent::StaminaConsumption, StaminaConsumptionRate, true, 0.0f);
}

void URogueLikeStaminaComponent::StopRunning()
{
    GetWorld()->GetTimerManager().ClearTimer(ReduceTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(RegenerationTimerHandle, this, &URogueLikeStaminaComponent::StaminaRegeneration,
        Owner->GetStaminaRegenerationRate(), true, RegenerationDelay);
}

void URogueLikeStaminaComponent::BeginPlay()
{
    Super::BeginPlay();
    Owner = Cast<ARogueLikeCharacter>(GetOwner());
    check(Owner);

    Owner->OnPassiveCharacteristicUpdated.AddDynamic(this, &ThisClass::UpdateRegenerationRate);
}

void URogueLikeStaminaComponent::UpdateRegenerationRate()
{
    GetWorld()->GetTimerManager().ClearTimer(RegenerationTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(
        RegenerationTimerHandle, this, &URogueLikeStaminaComponent::StaminaRegeneration, Owner->GetStaminaRegenerationRate(), true, 0.0f);
}
void URogueLikeStaminaComponent::StaminaRegeneration()
{
    SetStamina(Stamina + RegenerationModifire);
    if (IsStaminaFull() && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(RegenerationTimerHandle);
    }
}

void URogueLikeStaminaComponent::StaminaConsumption()
{
    if (!Owner || Owner->IsDead() || !GetWorld()) return;
    SetStamina(Stamina - ConsumptionModifire);

    if (IsStaminaConsumed())
    {
        OnStaminaConsumed.Broadcast();
        StopRunning();
    }
}

void URogueLikeStaminaComponent::SetStamina(const float NewStamina)
{
    if (!Owner) return;

    Stamina = FMath::Clamp(NewStamina, 0.0f, Owner->GetMaxStamina());
    OnStaminaUpdate.Broadcast();
}
