// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SDTCollectible.h"
#include "SDTFleeLocation.h"
#include <NavigationSystem.h>
#include <NavigationPath.h>
#include "SDTPathFollowingComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
//#include "UnrealMathUtility.h"
#include "SDTUtils.h"
#include "EngineUtils.h"

ASDTAIController::ASDTAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<USDTPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
}

AActor* ASDTAIController::GetClosestCollectible(APawn* pawn, TArray < AActor* > actors) {
    AActor* closestActor = NULL;
    float minDist = MAX_FLT;
    
    while (actors.Num() > 0) {
        AActor* actor = actors.Pop();

        if (FVector::DistXY(pawn->GetActorLocation(), actor->GetActorLocation()) < minDist) {
            if (!static_cast<ASDTCollectible*>(actor)->IsOnCooldown()) {
                closestActor = actor;
                minDist = FVector::DistXY(pawn->GetActorLocation(), closestActor->GetActorLocation());
            }
        }
    }
    return closestActor;
}

bool ASDTAIController::VerifyCollectibleCooldown(TArray<AActor*> collectibles, FNavPathPoint collectionLocation) {
    for (AActor* collectible : collectibles) {
        float dist = FVector::DistXY(collectionLocation.Location, static_cast<ASDTCollectible*>(collectible)->GetActorLocation());
        if (dist < 1.f) {
            UE_LOG(LogTemp, Log, TEXT("ChangeCollectible"));
            return static_cast<ASDTCollectible*>(collectible)->IsOnCooldown();
        }
    }
    return false;
}

void ASDTAIController::GoToBestTarget(float deltaTime)
{
    //Move to target depending on current behavior
    UWorld* const world = GetWorld();
    APawn* pawn = GetPawn();
    USDTPathFollowingComponent* pf = static_cast<USDTPathFollowingComponent*>(GetDefaultSubobjectByName(TEXT("PathFollowingComponent")));
    TArray < AActor* > OutActors;
    UGameplayStatics::GetAllActorsOfClass(world, ASDTCollectible::StaticClass(), OutActors);

    if (!pf->GetPath() || pf->GetPath() == NULL || VerifyCollectibleCooldown(OutActors, pf->GetPath()->GetPathPoints().Last())) {
        
        AActor* actor = GetClosestCollectible(pawn, OutActors);
        pf->ResetMove();
        FVector location = actor->GetActorLocation();
        UNavigationPath* path = UNavigationSystemV1::FindPathToLocationSynchronously(world, pawn->GetActorLocation(), location);
        pf->SetPath(path->GetPath());
        
    }
    ShowNavigationPath();
    pf->SetMoveSegment(pf->GetMoveSegmentStartIndex());
    pf->FollowPathSegment(deltaTime);
    pawn->AddMovementInput(pf->destination, 0.1f);
}

void ASDTAIController::OnMoveToTarget()
{
    m_ReachedTarget = false;
}

void ASDTAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    m_ReachedTarget = true;
}

void ASDTAIController::ShowNavigationPath()
{
    //Show current navigation path DrawDebugLine and DrawDebugSphere
    USDTPathFollowingComponent* pf = static_cast<USDTPathFollowingComponent*>(GetDefaultSubobjectByName(TEXT("PathFollowingComponent")));
    const TArray<FNavPathPoint>& points = pf->GetPath()->GetPathPoints();
    FVector start = GetPawn()->GetActorLocation();
    for (int i = pf->GetMoveSegmentStartIndex()+1; i < points.Num(); ++i) {
        DrawDebugLine(GetWorld(), start, points[i].Location, FColor::Black);
        start = points[i].Location;
    }

}

void ASDTAIController::ChooseBehavior(float deltaTime)
{
    UpdatePlayerInteraction(deltaTime);
}

void ASDTAIController::UpdatePlayerInteraction(float deltaTime)
{
    //finish jump before updating AI state
    if (AtJumpSegment)
        return;

    APawn* selfPawn = GetPawn();
    if (!selfPawn)
        return;

    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!playerCharacter)
        return;

    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * m_DetectionCapsuleForwardStartingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * m_DetectionCapsuleHalfLength * 2;

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_COLLECTIBLE));
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));

    TArray<FHitResult> allDetectionHits;
    GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(m_DetectionCapsuleRadius));

    FHitResult detectionHit;
    GetHightestPriorityDetectionHit(allDetectionHits, detectionHit);

    //Set behavior based on hit

    DrawDebugCapsule(GetWorld(), detectionStartLocation + m_DetectionCapsuleHalfLength * selfPawn->GetActorForwardVector(), m_DetectionCapsuleHalfLength, m_DetectionCapsuleRadius, selfPawn->GetActorQuat() * selfPawn->GetActorUpVector().ToOrientationQuat(), FColor::Blue);
}

void ASDTAIController::GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit)
{
    for (const FHitResult& hit : hits)
    {
        if (UPrimitiveComponent* component = hit.GetComponent())
        {
            if (component->GetCollisionObjectType() == COLLISION_PLAYER)
            {
                //we can't get more important than the player
                outDetectionHit = hit;
                return;
            }
            else if (component->GetCollisionObjectType() == COLLISION_COLLECTIBLE)
            {
                outDetectionHit = hit;
            }
        }
    }
}

void ASDTAIController::AIStateInterrupted()
{
    StopMovement();
    m_ReachedTarget = true;
}