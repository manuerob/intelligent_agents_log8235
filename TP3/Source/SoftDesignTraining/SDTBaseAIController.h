// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "SDTBaseAIController.generated.h"

/**
 * 
 */

UCLASS()
class SOFTDESIGNTRAINING_API ASDTBaseAIController : public AAIController
{
	GENERATED_BODY()

public:

    // Called when the game starts or when spawned
    ASDTBaseAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    virtual void Tick(float deltaTime) override;
    virtual void UpdatePlayerInteraction(float deltaTime) {};
    bool updated = false;

protected:
    virtual void RotationUpdate(float deltaTime) {};
    virtual void ImpulseToDirection(float deltaTime) {};
    
    bool m_ReachedTarget;
private:
    virtual void GoToBestTarget(float deltaTime) {};
    virtual void ShowNavigationPath() {};
    virtual void DrawCPUTimes() {};
    virtual void ResetTimer() {};
};