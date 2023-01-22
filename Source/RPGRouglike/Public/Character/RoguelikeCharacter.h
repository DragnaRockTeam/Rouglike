// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RogueLikeCoreTypes.h"
#include "RoguelikeCharacter.generated.h"

class URogueLikeHealthComponent;
class URogueLikeStaminaComponent;
class URogueLikeWeaponComponent;

UCLASS()
class RPGROUGLIKE_API ARogueLikeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ARogueLikeCharacter(const FObjectInitializer& ObjInit);
    FName GetWeaponRowName() const { return WeaponRowName; };
    bool IsDead() const { return bIsDead; }
    UFUNCTION(BlueprintCallable, Category = "Animations")
    float GetMovementDirection() const;

    // returns the Yaw rotation
    UFUNCTION(BlueprintCallable, Category = "Animations")
    float GetCursorDirection() const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    FVector GetCursorImpactPoint() const;

#pragma region PassiveCharacteristickGetters
    float GetMaxHealth() const { return PassiveCharacteristic->MaxHealth; }
    float GetHealthRegenerationRate() const { return PassiveCharacteristic->HealthRegeneration; }
    float GetMaxStamina() const { return PassiveCharacteristic->MaxStamina; }
    float GetStaminaRegenerationRate() const { return PassiveCharacteristic->StaminaRegeneration; }
    float GetDamageResistance() const { return PassiveCharacteristic->DamageResistance; }

    float GetAttackSpeed() const { return PassiveCharacteristic->AttackSpeed; }
    float GetDamageAmount() const { return PassiveCharacteristic->Damage; };
    float GetProjectileSpeed() const { return PassiveCharacteristic->ProjectileSpeed; }

    TMap<EAttackEffectChance, int32> GetEffectChances() const { return PassiveCharacteristic->EffectChances; }

    float GetMovementSpeed() const { return PassiveCharacteristic->MovementSpeed; }
    float GetReducingSkillsCooldownTime() const { return PassiveCharacteristic->ReducingSkillsCooldownTime; }
    TMap<EAttackEffect, float> GetEffectsDamages() const { return PassiveCharacteristic->EffectsDamages; }

    float GetVampirismPrecent() const { return PassiveCharacteristic->VampirismPrecent; }
#pragma endregion

    void AddPassiveCharacteristic(const FPassiveCharacteristic* AddCharacteristic);
    void InitCharacterData();

    UFUNCTION(BlueprintCallable, Category = "Movement")
    bool IsRunning() const;

    UPROPERTY(BlueprintAssignable, Category = "CharacterDelegates")
    FOnPassiveCharacteristicUpdated OnPassiveCharacteristicUpdated;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    FName WeaponRowName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    URogueLikeWeaponComponent* WeaponComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    URogueLikeHealthComponent* HealthComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    URogueLikeStaminaComponent* StaminaComponent;

    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    FCharacterData* CharacterData;
    FPassiveCharacteristic* PassiveCharacteristic;

    bool bIsMovingForward = false;
    bool bIsRunning = false;
    bool bIsDead = false;
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Run();

    UFUNCTION()
    void StopRunning();

    UFUNCTION()
    void OnDeath();
};
