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

    if (MoveSegmentStartIndex + 1 >= points.Num()) {
        destination = points.Last();
        ResetMove();
        return;
    }

    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];
    const FNavPathPoint& segmentEnd = points[MoveSegmentStartIndex + 1];

    
    if (SDTUtils::HasJumpFlag(segmentStart))
    {
        //update jump
        JumpFlag = true;
    }
    else
    {
        //update navigation along path
        JumpFlag = false;
    }
    destination = (segmentEnd.Location - GetCurrentNavLocation().Location).GetSafeNormal();
}

void USDTPathFollowingComponent::SetMoveSegment(int32 segmentStartIndex)
{
    Super::SetMoveSegment(segmentStartIndex);

    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];

    if (MoveSegmentStartIndex + 1 >= points.Num()) {
        return;
    }

    if (SDTUtils::HasJumpFlag(segmentStart))
    {
        //Handle starting jump
        UE_LOG(LogTemp, Log, TEXT("SetMoveSegment Jump"));
    }
    else
    {
        //Handle normal segments
        FVector2D position2D(GetCurrentNavLocation().Location);
        FVector2D destination2D(points[MoveSegmentStartIndex + 1]);
        if (position2D.Equals(destination2D, 10.f)) {
            MoveSegmentStartIndex++;
        }
    }
}

int32 USDTPathFollowingComponent::GetMoveSegmentStartIndex() {
    return MoveSegmentStartIndex;
}