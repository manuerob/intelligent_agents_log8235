// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsHelpers.h"
#include "SoftDesignTraining.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"

PhysicsHelpers::PhysicsHelpers(UWorld* world)
    : m_world(world)
{
}

PhysicsHelpers::~PhysicsHelpers()
{
}

bool PhysicsHelpers::SphereOverlap(const FVector& pos, float radius, TArray<struct FOverlapResult>& outOverlaps, bool drawDebug, ECollisionChannel collisionChannel)
{
    if (m_world == nullptr)
        return false;

    if (drawDebug)
        DrawDebugSphere(m_world, pos, radius, 24, FColor::Green);

	FCollisionObjectQueryParams objectQueryParams;
    FCollisionShape collisionShape;
    collisionShape.SetSphere(radius);
    FCollisionQueryParams queryParams = FCollisionQueryParams::DefaultQueryParam;
    queryParams.bReturnPhysicalMaterial = true;

	if (collisionChannel) {
		objectQueryParams.AddObjectTypesToQuery(collisionChannel);
	}

	m_world->OverlapMultiByObjectType(outOverlaps, pos, FQuat::Identity, objectQueryParams, collisionShape, queryParams);

    //Draw overlap results
    if (drawDebug)
    {
        for (int32 i = 0; i < outOverlaps.Num(); ++i)
        {
            if (outOverlaps[i].GetComponent())
                DebugDrawPrimitive(*(outOverlaps[i].GetComponent()));
        }
    }

    return outOverlaps.Num() > 0;
}

void PhysicsHelpers::DebugDrawPrimitive(const UPrimitiveComponent& primitive)
{
    FVector center = primitive.Bounds.Origin;
    FVector extent = primitive.Bounds.BoxExtent;

    DrawDebugBox(m_world, center, extent, FColor::Red);
}