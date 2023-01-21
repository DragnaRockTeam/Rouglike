// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RogueLikeCoreTypes.h"
#include "RogueLikeStaminaComponent.generated.h"

class ARoguelikeCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPGROUGLIKE_API URogueLikeStaminaComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URogueLikeStaminaComponent();

    UFUNCTION(BlueprintCallable)
    bool CanRun() const { return !IsStaminaConsumed(); }

    UFUNCTION(BlueprintCallable)
    bool IsStaminaConsumed() const { return FMath::IsNearlyZero(Stamina); }

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    float GetStaminaPrecent() const;

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    float GetStamina() const { return Stamina; };

    bool IsStaminaFull() const;

    UPROPERTY(BlueprintAssignable, Category = "Stamina properties")
    FOnStaminaUpdate OnStaminaUpdate;

    FOnStaminaConsumed OnStaminaConsumed;

    void StartRunning();
    void StopRunning();

protected:
    virtual void BeginPlay() override;
    // Delay after run, after which regeneration begins
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina properties")
    float RegenerationDelay = 5.f;

    //Can be only > 0
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina properties",
        meta = (ClampMin = 0.f, ClampMax = 999.f, UIMin = 0.f, UIMax = 999.f))
    float RegenerationModifire = 5.f;

    //Can be only > 0
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina properties",
        meta = (ClampMin = 0.f, ClampMax = 999.f, UIMin = 0.f, UIMax = 999.f))
    float ConsumptionModifire = 5.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina properties",
        meta = (ClampMin = 1.f, ClampMax = 999.f, UIMin = 1.f, UIMax = 999.f))
    float StaminaConsumptionRate = 0.5f;

private:
    float Stamina = 0.f;
    float StaminaRegenerationRate = 0.f;
    bool bIsRunning = false;

    FTimerHandle RegenerationTimerHandle;
    FTimerHandle ReduceTimerHandle;
    ARoguelikeCharacter* Owner;

    UFUNCTION()
    void UpdateRegenerationRate();

    void StaminaRegeneration();
    void StaminaConsumption();
    void SetStamina(const float NewStamina);
};
