// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTPathFollowingComponent.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SDTAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DrawDebugHelpers.h"

USDTPathFollowingComponent::USDTPathFollowingComponent(const FObjectInitializer& ObjectInitializer)
{

}

void USDTPathFollowingComponent::SetPath(FNavPathSharedPtr path) {
    Path = path;
}

void USDTPathFollowingComponent::ResetMove() {
    Path = NULL;
    MoveSegmentStartIndex = 0;
}

void USDTPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    if (MoveSegmentStartIndex+1 >= points.Num()) {
        destination = points.Last();
        ResetMove();
        return;
    }
    for (int i = 0; i < points.Num(); ++i) {
    }
    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];
    const FNavPathPoint& segmentEnd = points[MoveSegmentStartIndex + 1];

    
    if (SDTUtils::HasJumpFlag(segmentStart))
    {
        //update jump
        UE_LOG(LogTemp, Log, TEXT("Jump1"));
    }
    else
    {
        //update navigation along path
        destination = segmentEnd.Location - GetCurrentNavLocation().Location;
        
    }
}

void USDTPathFollowingComponent::SetMoveSegment(int32 segmentStartIndex)
{
    Super::SetMoveSegment(segmentStartIndex);

    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];

    if (SDTUtils::HasJumpFlag(segmentStart) && FNavMeshNodeFlags(segmentStart.Flags).IsNavLink())
    {
        //Handle starting jump
        UE_LOG(LogTemp, Log, TEXT("Jump2"));
    }
    else
    {
        //Handle normal segments
        FVector2D position2D(GetCurrentNavLocation().Location);
        FVector2D destination2D(points[MoveSegmentStartIndex+1]);
        if (position2D.Equals(destination2D, 10.f)) {
            MoveSegmentStartIndex++;
        }
    }
}

int32 USDTPathFollowingComponent::GetMoveSegmentStartIndex() {
    return MoveSegmentStartIndex;
}