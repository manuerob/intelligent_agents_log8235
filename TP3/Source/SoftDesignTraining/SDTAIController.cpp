// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include <string>
#include "SoftDesignTraining.h"
#include "SDTFleeLocation.h"
#include "SDTPathFollowingComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "SDTUtils.h"
#include "EngineUtils.h"
#include <SoftDesignTraining/SoftDesignTrainingMainCharacter.h>

ASDTAIController::ASDTAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<USDTPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
    //m_PlayerInteractionBehavior = PlayerInteractionBehavior_Collect;
    m_blackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void ASDTAIController::GoToBestTarget(float deltaTime)
{
    switch (m_PlayerInteractionBehavior)
    {
    case PlayerInteractionBehavior_Collect:

        FindRandomCollectible();

        break;

    case PlayerInteractionBehavior_Chase:

        MoveToPlayer();

        break;

    case PlayerInteractionBehavior_Flee:

        MoveToBestFleeLocation();

        break;
    }
}

void ASDTAIController::FindRandomCollectible()
{
    double start = FPlatformTime::Seconds();

    float closestSqrCollectibleDistance = 18446744073709551610.f;
    ASDTCollectible* closestCollectible = nullptr;

    TArray<AActor*> foundCollectibles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

	//UE_LOG(LogTemp, Log, TEXT(":^)"));

    while (foundCollectibles.Num() != 0)
    {
        int index = FMath::RandRange(0, foundCollectibles.Num() - 1);

        ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(foundCollectibles[index]);
        double end = FPlatformTime::Seconds();

        choiceCollectibleTime_ = FString("choose collect: ") + FString::SanitizeFloat(end - start);

        if (!collectibleActor)
            return;

        if (!collectibleActor->IsOnCooldown())
        {
			foundCollectible = collectibleActor;
			m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("HasPowerUpLoc"), true);
			MoveToCollectible();

			m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsFleeing"), false);
			m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsChasing"), false);
			m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsCollecting"), true);

            return;
        }
        else
        {
            foundCollectibles.RemoveAt(index);
        }
    }
}

void ASDTAIController::MoveToCollectible()
{
	MoveToLocation(foundCollectible->GetActorLocation(), 0.5f, false, true, true, NULL, false);
	OnMoveToTarget();
}

void ASDTAIController::MoveToPlayer()
{
	if (foundCollectible != nullptr)
	{
		m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("HasPowerUpLoc"), false);
		foundCollectible = nullptr;
	}

	m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsFleeing"), false);
	m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsChasing"), true);
	m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsCollecting"), false);

    ACharacter * playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!playerCharacter)
        return;

    MoveToActor(playerCharacter, 0.5f, false, true, true, NULL, false);
    OnMoveToTarget();
}

void ASDTAIController::PlayerInteractionLoSUpdate()
{
    ACharacter * playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!playerCharacter)
        return;

    TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
    TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
    TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));

    FHitResult losHit;
    GetWorld()->LineTraceSingleByObjectType(losHit, GetPawn()->GetActorLocation(), playerCharacter->GetActorLocation(), TraceObjectTypes);

    bool hasLosOnPlayer = false;

    if (losHit.GetComponent())
    {
        if (losHit.GetComponent()->GetCollisionObjectType() == COLLISION_PLAYER)
        {
            hasLosOnPlayer = true;
        }
    }

    if (hasLosOnPlayer)
    {
        if (GetWorld()->GetTimerManager().IsTimerActive(m_PlayerInteractionNoLosTimer))
        {
            GetWorld()->GetTimerManager().ClearTimer(m_PlayerInteractionNoLosTimer);
            m_PlayerInteractionNoLosTimer.Invalidate();
            DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), "Got LoS", GetPawn(), FColor::Red, 5.f, false);
        }
    }
    else
    {
        if (!GetWorld()->GetTimerManager().IsTimerActive(m_PlayerInteractionNoLosTimer))
        {
            GetWorld()->GetTimerManager().SetTimer(m_PlayerInteractionNoLosTimer, this, &ASDTAIController::OnPlayerInteractionNoLosDone, 2.f, false);

            DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), "Lost LoS", GetPawn(), FColor::Red, 5.f, false);
        }
    }
    
}

void ASDTAIController::OnPlayerInteractionNoLosDone()
{
    GetWorld()->GetTimerManager().ClearTimer(m_PlayerInteractionNoLosTimer);
    DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), "TIMER DONE", GetPawn(), FColor::Red, 5.f, false);

    if (!AtJumpSegment && !m_blackboardComponent->GetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsInChaseGroup")))
    {
        AIStateInterrupted();
        //m_PlayerInteractionBehavior = PlayerInteractionBehavior_Collect;
    }
}

void ASDTAIController::MoveToBestFleeLocation()
{

	m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsFleeing"), true);
	m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsChasing"), false);
	m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsCollecting"), false);

	if (foundCollectible != nullptr)
	{
		m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("HasPowerUpLoc"), false);
		foundCollectible = nullptr;
	}

    float bestLocationScore = 0.f;
    ASDTFleeLocation* bestFleeLocation = nullptr;

    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!playerCharacter)
        return;

    double start = FPlatformTime::Seconds();

    for (TActorIterator<ASDTFleeLocation> actorIterator(GetWorld(), ASDTFleeLocation::StaticClass()); actorIterator; ++actorIterator)
    {
        ASDTFleeLocation* fleeLocation = Cast<ASDTFleeLocation>(*actorIterator);
        if (fleeLocation)
        {
            float distToFleeLocation = FVector::Dist(fleeLocation->GetActorLocation(), playerCharacter->GetActorLocation());

            FVector selfToPlayer = playerCharacter->GetActorLocation() - GetPawn()->GetActorLocation();
            selfToPlayer.Normalize();

            FVector selfToFleeLocation = fleeLocation->GetActorLocation() - GetPawn()->GetActorLocation();
            selfToFleeLocation.Normalize();

            float fleeLocationToPlayerAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(selfToPlayer, selfToFleeLocation)));
            float locationScore = distToFleeLocation + fleeLocationToPlayerAngle * 100.f;

            if (locationScore > bestLocationScore)
            {
                bestLocationScore = locationScore;
                bestFleeLocation = fleeLocation;
            }

            DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), FString::SanitizeFloat(locationScore), fleeLocation, FColor::Red, 5.f, false);
        }
    }
    double end = FPlatformTime::Seconds();
    choiceFleeLocationTime_ = FString("Flee loc: ") + FString::SanitizeFloat(end - start);

    if (bestFleeLocation)
    {
        MoveToLocation(bestFleeLocation->GetActorLocation(), 0.5f, false, true, false, NULL, false);
        OnMoveToTarget();
    }
}

void ASDTAIController::OnMoveToTarget()
{
    m_ReachedTarget = false;
}

void ASDTAIController::RotateTowards(const FVector& targetLocation)
{
    if (!targetLocation.IsZero())
    {
        FVector direction = targetLocation - GetPawn()->GetActorLocation();
        FRotator targetRotation = direction.Rotation();

        targetRotation.Yaw = FRotator::ClampAxis(targetRotation.Yaw);

        SetControlRotation(targetRotation);
    }
}

void ASDTAIController::SetActorLocation(const FVector& targetLocation)
{
    GetPawn()->SetActorLocation(targetLocation);
}

void ASDTAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    m_ReachedTarget = true;
}

void ASDTAIController::ShowNavigationPath()
{
    if (UPathFollowingComponent* pathFollowingComponent = GetPathFollowingComponent())
    {
        if (pathFollowingComponent->HasValidPath())
        {
            const FNavPathSharedPtr path = pathFollowingComponent->GetPath();
            TArray<FNavPathPoint> pathPoints = path->GetPathPoints();

            for (int i = 0; i < pathPoints.Num(); ++i)
            {
                DrawDebugSphere(GetWorld(), pathPoints[i].Location, 10.f, 8, FColor::Yellow);

                if (i != 0)
                {
                    DrawDebugLine(GetWorld(), pathPoints[i].Location, pathPoints[i - 1].Location, FColor::Yellow);
                }
            }
        }
    }
}

void ASDTAIController::UpdatePlayerInteraction(float deltaTime)
{
    if (!m_canUpdate) {
        return;
    }
    m_doneUpdate = false;
    //finish jump before updating AI state
	if (AtJumpSegment) {
		UE_LOG(LogTemp, Log, TEXT(":^("))
        return;
	}

    APawn* selfPawn = GetPawn();
    if (!selfPawn)
        return;

    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!playerCharacter)
        return;

    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * m_DetectionCapsuleForwardStartingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * m_DetectionCapsuleHalfLength * 2;

    double start = FPlatformTime::Seconds();

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));

    TArray<FHitResult> allDetectionHits;
    GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(m_DetectionCapsuleRadius));

    FHitResult detectionHit;
    GetHightestPriorityDetectionHit(allDetectionHits, detectionHit);

    double end = FPlatformTime::Seconds();
    detectPlayerTime_ = FString("Detect p: ") + FString::SanitizeFloat(end-start);
   
    DrawDebugString(GetWorld(), FVector(0.f, 0.f, 300.f), detectPlayerTime_, GetPawn(), FColor::Orange, 0.f, false);
    DrawDebugString(GetWorld(), FVector(0.f, 0.f, 200.f), choiceFleeLocationTime_, GetPawn(), FColor::Orange, 0.f, false);
    DrawDebugString(GetWorld(), FVector(0.f, 0.f, 100.f), choiceCollectibleTime_, GetPawn(), FColor::Orange, 0.f, false);

    //PlayerInteractionLoSUpdate();

    UpdatePlayerInteractionBehavior(detectionHit, deltaTime);


    if (GetMoveStatus() == EPathFollowingStatus::Idle)
    {
        m_ReachedTarget = true;

		m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsFleeing"), false);
		m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsChasing"), false);
		m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsCollecting"), false);

		if (foundCollectible != nullptr)
		{
			m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("HasPowerUpLoc"), false);
			foundCollectible = nullptr;
		}
    }

	ASoftDesignTrainingMainCharacter* mainCharacter = static_cast<ASoftDesignTrainingMainCharacter*>(playerCharacter);

	if (m_blackboardComponent->GetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsInChaseGroup")) && !mainCharacter->chaseGroup.Contains(static_cast<ASoftDesignTrainingCharacter*>(GetPawn()))) {
		m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsInChaseGroup"), false);
	}

	if (m_blackboardComponent->GetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsPlayerSeen")) && !mainCharacter->activelySeeingGroup.Contains(static_cast<ASoftDesignTrainingCharacter*>(GetPawn()))) {
		m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsPlayerSeen"), false);
	}

	//UE_LOG(LogTemp, Log, TEXT("%i"), mainCharacter->chaseGroup.Num());
/*
    FString debugString = "";

    switch (m_PlayerInteractionBehavior)
    {
    case PlayerInteractionBehavior_Chase:
        debugString = "Chase";
        break;
    case PlayerInteractionBehavior_Flee:
        debugString = "Flee";
        break;
    case PlayerInteractionBehavior_Collect:
        debugString = "Collect";
        break;
    }

    DrawDebugString(GetWorld(), FVector(0.f, 0.f, 5.f), debugString, GetPawn(), FColor::Orange, 0.f, false);*/

    DrawDebugCapsule(GetWorld(), detectionStartLocation + m_DetectionCapsuleHalfLength * selfPawn->GetActorForwardVector(), m_DetectionCapsuleHalfLength, m_DetectionCapsuleRadius, selfPawn->GetActorQuat() * selfPawn->GetActorUpVector().ToOrientationQuat(), FColor::Blue);
    m_doneUpdate = true;
}

bool ASDTAIController::HasLoSOnHit(const FHitResult& hit)
{
    if (!hit.GetComponent())
        return false;

    TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
    TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

    FVector hitDirection = hit.ImpactPoint - hit.TraceStart;
    hitDirection.Normalize();

    FHitResult losHit;
    FCollisionQueryParams queryParams = FCollisionQueryParams();
    queryParams.AddIgnoredActor(hit.GetActor());

    GetWorld()->LineTraceSingleByObjectType(losHit, hit.TraceStart, hit.ImpactPoint + hitDirection, TraceObjectTypes, queryParams);

    return losHit.GetActor() == nullptr;
}

void ASDTAIController::AIStateInterrupted()
{
    StopMovement();
    m_ReachedTarget = true;
	m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsFleeing"), false);
	m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsChasing"), false);
	m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsCollecting"), false);
}

ASDTAIController::PlayerInteractionBehavior ASDTAIController::GetCurrentPlayerInteractionBehavior(const FHitResult& hit)
{

    if (m_PlayerInteractionBehavior == PlayerInteractionBehavior_Collect)
    {
		if (!hit.GetComponent() || hit.GetComponent()->GetCollisionObjectType() != COLLISION_PLAYER || !HasLoSOnHit(hit)) {
		
			return PlayerInteractionBehavior_Collect;
		}

		if (SDTUtils::IsPlayerPoweredUp(GetWorld())) {
			return PlayerInteractionBehavior_Flee;
		}
		else {
			return PlayerInteractionBehavior_Chase;
		}

    }
    else
    {
        PlayerInteractionLoSUpdate();

		if (SDTUtils::IsPlayerPoweredUp(GetWorld())) {
			return PlayerInteractionBehavior_Flee;
		}
		else {
			return PlayerInteractionBehavior_Chase;
		}
    }
}

void ASDTAIController::GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit)
{
    for (const FHitResult& hit : hits)
    {
        if (UPrimitiveComponent* component = hit.GetComponent())
        {
			if (component->GetCollisionObjectType() == COLLISION_PLAYER && HasLoSOnHit(hit))
			{
				//we can't get more important than the player
				outDetectionHit = hit;

				m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsPlayerPoweredUp"), SDTUtils::IsPlayerPoweredUp(GetWorld()));

				if (!SDTUtils::IsPlayerPoweredUp(GetWorld())) {
					
					m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsPlayerSeen"), true);
					m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsInChaseGroup"), true);

					ASoftDesignTrainingMainCharacter* mainCharacter = static_cast<ASoftDesignTrainingMainCharacter*>(hit.GetActor());

					if (!mainCharacter->activelySeeingGroup.Contains(static_cast<ASoftDesignTrainingCharacter*>(GetPawn())))
					{
						mainCharacter->activelySeeingGroup.Add(static_cast<ASoftDesignTrainingCharacter*>(GetPawn()));
					}

					if (!mainCharacter->chaseGroup.Contains(static_cast<ASoftDesignTrainingCharacter*>(GetPawn())))
					{
						mainCharacter->chaseGroup.Add(static_cast<ASoftDesignTrainingCharacter*>(GetPawn()));
					}
				}
				
                return;
			}
			else if (component->GetCollisionObjectType() == COLLISION_COLLECTIBLE)
			{
				outDetectionHit = hit;
			}
        }
    }

    if (m_blackboardComponent->GetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsInChaseGroup"))) {
        
        ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        ASoftDesignTrainingMainCharacter* mainCharacter = static_cast<ASoftDesignTrainingMainCharacter*>(playerCharacter);
        
		if (mainCharacter->activelySeeingGroup.Contains(static_cast<ASoftDesignTrainingCharacter*>(GetPawn())))
		{
			mainCharacter->activelySeeingGroup.Remove(static_cast<ASoftDesignTrainingCharacter*>(GetPawn()));
		}

        if (mainCharacter->activelySeeingGroup.Num() == 0) {
            mainCharacter->chaseGroup.Empty();
            m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsInChaseGroup"), false);
        }
    }

	m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_blackboardComponent->GetKeyID("IsPlayerSeen"), false);
}

void ASDTAIController::UpdatePlayerInteractionBehavior(const FHitResult& detectionHit, float deltaTime)
{
    PlayerInteractionBehavior currentBehavior = GetCurrentPlayerInteractionBehavior(detectionHit);

    if (currentBehavior != m_PlayerInteractionBehavior)
    {
        m_PlayerInteractionBehavior = currentBehavior;
        AIStateInterrupted();
    }
}

void ASDTAIController::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    Super::EndPlay(EndPlayReason);

}

FVector ASDTAIController::GetPawnLocation()
{
	return GetPawn()->GetActorLocation();
}
