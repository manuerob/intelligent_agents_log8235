// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PhysicsHelpers.h"

#include "SDTAIController.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()

private:
    float const _maxScale = 1.0f;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float _acceleration;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float _scale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    FVector _direction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float _collisionDistance;


    virtual void Tick(float deltaTime) override;
    void SetSpeedVector(FVector dir);
    void Accelerate();
    void LookForWall(FVector dir);
    bool ChangedDirection(FVector currentDir, FVector newDir);
    PhysicsHelpers GetPhysicsHelpers();
};
