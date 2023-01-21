// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RogueLikeCoreTypes.h"
#include "RogueLikeAnimNotify.generated.h"

/**
 *
 */
UCLASS()
class RPGROUGLIKE_API URogueLikeAnimNotify : public UAnimNotify
{
    GENERATED_BODY()
public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

    FOnNotifiedSignature OnNotified;
};
