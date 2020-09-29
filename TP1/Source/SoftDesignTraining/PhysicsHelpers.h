// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//Forward
class UWorld;

/**
 *
 */
class SOFTDESIGNTRAINING_API PhysicsHelpers
{
public:
    PhysicsHelpers(UWorld* world);
    ~PhysicsHelpers();

    bool SphereOverlap(const FVector& pos, float radius, TArray<struct FOverlapResult>& outOverlaps, bool drawdebug, ECollisionChannel collisionChannel);

protected:
    void DebugDrawPrimitive(const UPrimitiveComponent& primitive);

    UWorld* m_world;
};
