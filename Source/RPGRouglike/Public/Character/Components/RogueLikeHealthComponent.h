// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RogueLikeCoreTypes.h"
#include "RoguelikeHealthComponent.generated.h"

class ARogueLikeCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPGROUGLIKE_API URogueLikeHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URogueLikeHealthComponent();
    UFUNCTION(BlueprintCallable)
    bool IsDead() const { return FMath::IsNearlyZero(Health); }

    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealth() const { return Health; }

    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealthPrecent() const;
    bool IsHealthFull() const;

    UPROPERTY(BlueprintAssignable, Category = "Health properties")
    FOnHealthUpdate OnHealthUpdate;
    FOnDeathSignature OnDeath;

protected:
    virtual void BeginPlay() override;

    // Delay after taking damage, after which regeneration begins
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health properties")
    float RegenerationDelay = 10.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health properties",
        meta = (ClampMin = 0.f, ClampMax = 999.f, UIMin = 0.f, UIMax = 999.f))
    float HealModifire = 1.f;

private:
    float Health = 0.f;
    float RegenerationRate = 0.0f;

    FTimerHandle HealTimerHandle;
    ARogueLikeCharacter* Owner;

    UFUNCTION()
    void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
        class AController* DamageInstigatedBy, AActor* DamageCauser);

    UFUNCTION()
    void UpdateHealRate();

    void HealUpdate();
    void SetHealth(float NewHealth);
    friend class ARogueLikeCharacter;
};
