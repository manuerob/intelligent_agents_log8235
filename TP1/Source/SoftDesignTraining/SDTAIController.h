// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "SDTAIController.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()

private:
    const float REVERSE_DIR = -1.0;
    float _speed;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float _acceleration;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float _maxSpeed = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float _detectionDistance = 200.0f;

    virtual void Tick(float deltaTime) override;

    void Accelerate(APawn* pawn);
    void SetSpeedVector(APawn* pawn, FVector dir);

    FVector GetNextDirection(APawn* pawn, UWorld* world);
    void RotatePawn(APawn* pawn, FRotator rotation);
    FRotator GetRotatorFromDirection(APawn* pawn, FVector newDir);

    bool RayCast(APawn* pawn, UWorld* world, const FVector& start, const FVector& end);
};
