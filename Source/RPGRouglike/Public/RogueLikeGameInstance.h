// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RogueLikeCoreTypes.h"
#include "RogueLikeGameInstance.generated.h"

/**
 *
 */
UCLASS()
class RPGROUGLIKE_API URogueLikeGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    int32 GetWeaponLevel() const { return WeaponLevel; }
    void SetWeaponLevel(int32 NewLevel) { WeaponLevel = NewLevel; }
    void SetCharacterDataRowName(FName SetCharacterDataRowName) { CharacterDataRowName = SetCharacterDataRowName; }
    FName GetCharacterDataRowName() const { return CharacterDataRowName; }

protected:
    virtual void Init() override;

private:
    int32 WeaponLevel = 1;
    FName CharacterDataRowName = FName("Default");
};
