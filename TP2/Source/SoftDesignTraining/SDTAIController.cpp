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

AActor* ASDTAIController::GetClosestActor(APawn* pawn, TArray < AActor* > actors, ActorType actorType) {
    AActor* closestActor = NULL;
    float minDist = MAX_FLT;
    
    while (actors.Num() > 0) {
        AActor* actor = actors.Pop();

        if (FVector::DistXY(pawn->GetActorLocation(), actor->GetActorLocation()) < minDist) {
			if (actorType == ActorType::COLLECTIBLE) {
				if (!static_cast<ASDTCollectible*>(actor)->IsOnCooldown()) {
					closestActor = actor;
					minDist = FVector::DistXY(pawn->GetActorLocation(), closestActor->GetActorLocation());
				}
			}
			else if (actorType == ActorType::FLEE_LOCATION) {
				closestActor = actor;
				minDist = FVector::DistXY(pawn->GetActorLocation(), closestActor->GetActorLocation());
			}
        }
    }
	_actorPos = closestActor->GetActorLocation();
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
    //TArray < AActor* > OutActors;
    //UGameplayStatics::GetAllActorsOfClass(world, ASDTCollectible::StaticClass(), OutActors);

    //if ((!pf->GetPath() || pf->GetPath() == NULL) && _targetActor != NULL) {
        //AActor* actor = GetClosestCollectible(pawn, OutActors);
        pf->ResetMove();
        UNavigationPath* path = UNavigationSystemV1::FindPathToLocationSynchronously(world, pawn->GetActorLocation(), _actorPos);
        pf->SetPath(path->GetPath());

		//m_ReachedTarget = false;
    //}
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
	
void ASDTAIController::GoToSelectedTarget(float deltaTime) {
	UWorld* const world = GetWorld();
	APawn* pawn = GetPawn();
	USDTPathFollowingComponent* pf = static_cast<USDTPathFollowingComponent*>(GetDefaultSubobjectByName(TEXT("PathFollowingComponent")));

	if (!(!pf->GetPath() || pf->GetPath() == NULL)) {
		pf->SetMoveSegment(pf->GetMoveSegmentStartIndex());
		pf->FollowPathSegment(deltaTime);
		GetPawn()->AddMovementInput(pf->destination, 0.1f);

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

	if (!(_player->IsPoweredUp()))
	{
		if (isPlayer && (!detectionHit.GetActor() || detectionHit.GetComponent()->GetCollisionObjectType() == COLLISION_COLLECTIBLE)) {
			float delta = FVector::Dist(GetPawn()->GetActorLocation(), _actorPos);
			//UE_LOG(LogTemp, Log, TEXT("%f"), delta);
			if (delta < 10.0) {
				isPlayer = false;
			}

			if (_player->isDead) {
				_player->isDead = false;
				isPlayer = false;
			}
		}
		else if (detectionHit.GetActor()) {
			_targetActor = detectionHit.GetActor();
			_actorPos = detectionHit.GetActor()->GetActorLocation();
			if (detectionHit.GetComponent()->GetCollisionObjectType() == COLLISION_PLAYER) {
				isPlayer = true;
			}
		}
		else {
			TArray < AActor* > OutActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), OutActors);
			_targetActor = GetClosestActor(GetPawn(), OutActors, ActorType::COLLECTIBLE);
		}
	}
	else
	{
		TArray < AActor* > OutActors;
	    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTFleeLocation::StaticClass(), OutActors);
		_targetActor = GetClosestActor(GetPawn(), OutActors, ActorType::FLEE_LOCATION);
	}

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
				FHitResult raycastHit;
				if (!GetWorld()->LineTraceSingleByObjectType(raycastHit, GetPawn()->GetActorLocation(), hit.GetActor()->GetActorLocation(), FCollisionObjectQueryParams().AllStaticObjects))
				{
					//we can't get more important than the player
					outDetectionHit = hit;
					return;
				}
            }
            else if (component->GetCollisionObjectType() == COLLISION_COLLECTIBLE)
            {
				if (!static_cast<ASDTCollectible*>(hit.GetActor())->IsOnCooldown())
				{
					FHitResult raycastHit;
					if (!GetWorld()->LineTraceSingleByObjectType(raycastHit, GetPawn()->GetActorLocation(), hit.GetActor()->GetActorLocation(), FCollisionObjectQueryParams().AllStaticObjects))
					{
						//we can't get more important than the player
						outDetectionHit = hit;
					}
				}
            }
        }
    }
}

void ASDTAIController::AIStateInterrupted()
{
    StopMovement();
    m_ReachedTarget = true;
}

bool ASDTAIController::GetPlayer() 
{
	ACharacter * playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	_player = static_cast<ASoftDesignTrainingMainCharacter*>(playerCharacter);
	return (_player != nullptr);
}