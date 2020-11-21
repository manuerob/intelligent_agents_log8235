// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/FSM/AI_FleeState.h"
#include "AI/FSM/AI_ChaseState.h"
#include "SoftDesignTraining.h"
#include "AIBase.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

AI_FleeState::AI_FleeState()
    :m_timeToStayInFlee(0.f)
{
    m_stateType = EStateType::Flee;
}

AI_FleeState::~AI_FleeState()
{
}

void AI_FleeState::OnEnter(EStateType fromStateId, AAIBase* aiOwner)
{
    m_timeToStayInFlee = 3.f;
    aiOwner->ProcessScaredDestination(m_fleeDestination, m_timeToStayInFlee);
}

void AI_FleeState::OnCreate()
{

}

void AI_FleeState::OnUpdate(float deltaTime, AAIBase* aiOwner)
{
    if (aiOwner) {
        if (m_timeToStayInFlee > 0)
        {
            m_timeToStayInFlee -= deltaTime;
            DrawDebugSphere(aiOwner->GetWorld(), aiOwner->GetActorLocation() + FVector(0.f, 0.f, 100.f), 15.0f, 32, FColor::Orange);
            if (m_fleeDestination != FVector::ZeroVector)
            {
                NavigateToPos(m_fleeDestination, aiOwner);
            }
        }
        else
        {
            aiOwner->SetAgentIsScared(false);
        }
    }
}

void AI_FleeState::OnExit(EStateType toStateId, AAIBase* aiOwner)
{

}

void AI_FleeState::OnEvaluateTransitions(bool& sucess, EStateType& transitionStateId, AAIBase* aiOwner)
{
    sucess = false;
    if (ShouldTransitionToChaseState(aiOwner))
    {
        transitionStateId = EStateType::Chase;
        sucess = true;
    }
    else if (ShouldTransitionToPatrolState(aiOwner))
    {
        transitionStateId = EStateType::Patrol;
        sucess = true;
    }
}

bool AI_FleeState::ShouldTransitionToPatrolState(AAIBase* aiOwner)
{
    return !aiOwner->IsAgentScared();
}

bool AI_FleeState::ShouldTransitionToChaseState(AAIBase* aiOwner)
{
    if (!aiOwner->IsAgentScared()) {
        aiOwner->DetectPlayer();
        return aiOwner->IsTargetPlayerSeen();
    }
    return false;
}

void AI_FleeState::NavigateToPos(const FVector& pos, AAIBase* aiOwner)
{
    UAIBlueprintHelperLibrary::SimpleMoveToLocation(aiOwner->GetController(), pos);
}
