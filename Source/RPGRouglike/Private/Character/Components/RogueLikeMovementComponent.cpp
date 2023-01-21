// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Components/RogueLikeMovementComponent.h"
#include "Character/RoguelikeCharacter.h"

float URogueLikeMovementComponent::GetMaxSpeed() const
{
    const float MaxSpeed = Super::GetMaxSpeed();
    const ARoguelikeCharacter* Player = Cast<ARoguelikeCharacter>(GetPawnOwner());
    float MovementMultiplier = Player ? Player->GetMovementSpeed() : 1.0f;
    return Player && Player->IsRunning() ? MaxSpeed * RunModifier * Player->GetMovementSpeed() * MovementMultiplier
                                         : MaxSpeed * MovementMultiplier;
}
