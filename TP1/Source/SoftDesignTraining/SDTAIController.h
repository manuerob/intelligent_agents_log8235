// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PhysicsHelpers.h"
#include "SDTCollectible.h"
#include "SDTUtils.h"

#include "SDTAIController.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()

private:
    const float REVERSE_DIR = -1.0f;
    float _speed = 0.0f;
    float _time = 0.0f;
	float _visionAngle = PI / 3.0f;
    float _yaw = 1.0f;
	enum PawnState { WANDERING, ROTATING, PICKING_POWERUP, CHASING };
	PawnState _currentState = WANDERING;
    FVector _directionGlob;
    ASDTCollectible* _powerUp;
	//FTimerHandle _powerUpTimer;
	//UMeshComponent* _pawnMaterial = nullptr;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float _a = 0.1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float _maxSpeed = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float _detectionDistance = 200.0f;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	//	float _powerUpDuration = 10.f;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	//	UMaterialInterface* _poweredUpMaterial;

    virtual void Tick(float deltaTime) override;

	void Wandering(float deltaTime, APawn* pawn, UWorld* world);
    void Rotating(APawn* pawn);

    void CalculateSpeed(APawn* pawn);
    void SetSpeedVector(APawn* pawn, FVector dir);

    FVector GetNextDirection(APawn* pawn, UWorld* world);
    FVector GetNextDirectionParallelToWorld(FVector direction);
    void RotatePawn(APawn* pawn, FRotator rotation);
    FRotator GetRotatorFromDirection(APawn* pawn, FVector newDir);

    void LocateObjects(APawn* pawn, UWorld* world);
	bool LocateDeathTrap(APawn* pawn, UWorld* world);
    void PickUpPowerUp(float deltaTime, APawn* pawn, UWorld* world);

    PhysicsHelpers GetPhysicsHelpers();

	bool IsInsideCone(APawn * pawn, AActor * targetActor) const;

    bool RayCast(APawn* pawn, UWorld* world, const FVector& start, const FVector& end);

	//void OnPowerUpDone();
};
