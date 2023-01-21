// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/RogueLikeAnimNotify.h"

void URogueLikeAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    OnNotified.Broadcast(MeshComp);
    Super::Notify(MeshComp, Animation);
}
