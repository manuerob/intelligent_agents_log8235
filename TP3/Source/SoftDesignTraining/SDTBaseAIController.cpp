// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTBaseAIController.h"
#include "SoftDesignTraining.h"

ASDTBaseAIController::ASDTBaseAIController(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    m_ReachedTarget = true;
}

void ASDTBaseAIController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // UE_LOG(LogTemp, Log, TEXT("Tick"));

    // DrawCPUTimes();

    ResetTimer();

    /*UpdatePlayerInteraction(deltaTime);

    if (m_ReachedTarget)
    {
        GoToBestTarget(deltaTime);
    }
    else
    {
        ShowNavigationPath();
    }*/
}

