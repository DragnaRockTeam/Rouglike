// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/RoguelikeCharacter.h"
#include "GameFramework/Controller.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"

#include "Character/Components/RogueLikeMovementComponent.h"
#include "Character/Components/RogueLikeHealthComponent.h"
#include "Character/Components/RogueLikeStaminaComponent.h"
#include "Character/Components/RogueLikeWeaponComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogCharacter, All, All);

ARoguelikeCharacter::ARoguelikeCharacter(const FObjectInitializer& ObjInit)
    : Super(ObjInit.SetDefaultSubobjectClass<URogueLikeMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = false;

    HealthComponent = CreateDefaultSubobject<URogueLikeHealthComponent>("HealthComponent");
    StaminaComponent = CreateDefaultSubobject<URogueLikeStaminaComponent>("StaminaComponent");
    WeaponComponent = CreateDefaultSubobject<URogueLikeWeaponComponent>("WeaponComponent");
}

void ARoguelikeCharacter::AddPassiveCharacteristic(const FPassiveCharacteristic* AddCharacteristic)
{
    PassiveCharacteristic.MaxHealth = FMath::Clamp(PassiveCharacteristic.MaxHealth + AddCharacteristic->MaxHealth, 1.0f, 10000.0f);
    PassiveCharacteristic.MaxStamina = FMath::Clamp(PassiveCharacteristic.MaxStamina + AddCharacteristic->MaxStamina, 1.0f, 10000.0f);

    PassiveCharacteristic.AttackSpeed = FMath::Clamp(PassiveCharacteristic.AttackSpeed + AddCharacteristic->AttackSpeed, 1.0f, 10.0f);
    PassiveCharacteristic.Damage = FMath::Clamp(PassiveCharacteristic.Damage + AddCharacteristic->Damage, 0.0f, 10000.f);
    PassiveCharacteristic.DamageResistance = FMath::Clamp(PassiveCharacteristic.AttackSpeed + AddCharacteristic->AttackSpeed, 0.0f, 1.0f);

    PassiveCharacteristic.HealthRegeneration =
        FMath::Clamp(PassiveCharacteristic.HealthRegeneration + AddCharacteristic->HealthRegeneration, 0.0f, 100.0f);
    PassiveCharacteristic.StaminaRegeneration =
        FMath::Clamp(PassiveCharacteristic.StaminaRegeneration + AddCharacteristic->StaminaRegeneration, 0.0f, 100.0f);
    PassiveCharacteristic.MovementSpeed = FMath::Clamp(PassiveCharacteristic.MovementSpeed + AddCharacteristic->MovementSpeed, 1.0f, 10.0f);

    for (const auto& EffectPair : PassiveCharacteristic.EffectChances)
    {
        PassiveCharacteristic.EffectChances[EffectPair.Key] =
            FMath::Clamp(EffectPair.Value + AddCharacteristic->EffectChances[EffectPair.Key], 0, 100);
    }
    for (const auto& EffectPair : PassiveCharacteristic.EffectsDamages)
    {
        PassiveCharacteristic.EffectsDamages[EffectPair.Key] =
            FMath::Clamp(EffectPair.Value + AddCharacteristic->EffectsDamages[EffectPair.Key], 0.0f, 10000.0f);
    }

    PassiveCharacteristic.ProjectileSpeed =
        FMath::Clamp(PassiveCharacteristic.ProjectileSpeed + AddCharacteristic->ProjectileSpeed, 1.0f, 10000.0f);
    PassiveCharacteristic.ReducingSkillsCooldownTime += AddCharacteristic->ReducingSkillsCooldownTime;

    OnPassiveCharacteristicUpdated.Broadcast();
    checkf(CheckPassiveCharacteristic(PassiveCharacteristic), TEXT("PassiveCharacteristic is not valid"));
}

void ARoguelikeCharacter::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogCharacter, All, TEXT("%s has valid characteristic: %s"), *GetName(),
        (CheckPassiveCharacteristic(PassiveCharacteristic) ? TEXT("true") : TEXT("false")));

    HealthComponent->OnDeath.AddDynamic(this, &ThisClass::OnDeath);
}

void ARoguelikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ThisClass::Run);
    PlayerInputComponent->BindAction("Run", IE_Released, this, &ThisClass::StopRunning);

    PlayerInputComponent->BindAction("Attack", IE_Pressed, WeaponComponent, &URogueLikeWeaponComponent::StartAttack);
    PlayerInputComponent->BindAction("Attack", IE_Released, WeaponComponent, &URogueLikeWeaponComponent::StopAttack);

    PlayerInputComponent->BindAxis("MoveForward", this, &ThisClass::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);
}

void ARoguelikeCharacter::MoveForward(float Value)
{
    if (!Controller) return;
    if (Value == 0.0f) return;

    bIsMovingForward = true;
    AddMovementInput(GetActorForwardVector(), Value);
}

void ARoguelikeCharacter::MoveRight(float Value)
{
    if (!Controller) return;
    if (Value == 0.0f) return;

    AddMovementInput(GetActorRightVector(), Value);
}

void ARoguelikeCharacter::Run()
{
    if (StaminaComponent->CanRun())
    {
        bIsRunning = true;
        StaminaComponent->StartRunning();
        StaminaComponent->OnStaminaConsumed.AddDynamic(this, &ThisClass::StopRunning);
    }
}

void ARoguelikeCharacter::StopRunning()
{
    bIsRunning = false;
    StaminaComponent->StopRunning();
    StaminaComponent->OnStaminaConsumed.RemoveDynamic(this, &ThisClass::StopRunning);
}

bool ARoguelikeCharacter::IsRunning() const
{
    return bIsRunning && !GetVelocity().IsZero() && bIsMovingForward;
}

float ARoguelikeCharacter::GetMovementDirection() const
{
    if (GetVelocity().IsZero()) return 0.0f;

    const auto VelocityNormal = GetVelocity().GetSafeNormal();
    const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
    const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);
    const auto Degrees = FMath::RadiansToDegrees(AngleBetween);

    return CrossProduct.IsZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

float ARoguelikeCharacter::GetCursorDirection() const
{
    return (UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetCursorImpactPoint())).Yaw;
}

FVector ARoguelikeCharacter::GetCursorImpactPoint() const
{
    if (!GetWorld()) return FVector();
    auto* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController) return FVector();
    TArray<TEnumAsByte<EObjectTypeQuery>> Objects = {EObjectTypeQuery::ObjectTypeQuery1};
    FHitResult HitResult;

    PlayerController->GetHitResultUnderCursorForObjects(Objects, true, HitResult);
    return HitResult.ImpactPoint;
}

void ARoguelikeCharacter::OnDeath()
{
    bIsDead = true;
    GetCharacterMovement()->DisableMovement();
    SetLifeSpan(3.f);

    if (Controller) Controller->ChangeState(NAME_Spectating);

    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);
}
