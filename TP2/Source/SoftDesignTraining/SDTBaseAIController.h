// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SDTBaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API ASDTBaseAIController : public AAIController
{
	GENERATED_BODY()

public:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
        float RunSpeed = 1.f;

    ASDTBaseAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    virtual void Tick(float deltaTime) override;
	
protected:
    virtual void RotationUpdate(float deltaTime) {};
    virtual void ImpulseToDirection(float deltaTime) {};

    bool m_ReachedTarget;
	bool m_FoundPlayer = false;
private:
    virtual void GoToBestTarget(float deltaTime) {};
    virtual void ChooseBehavior(float deltaTime) {};
    virtual void ShowNavigationPath() {};
	virtual void GoToSelectedTarget(float deltaTime) {};
	virtual bool GetPlayer() { return false; };
};
