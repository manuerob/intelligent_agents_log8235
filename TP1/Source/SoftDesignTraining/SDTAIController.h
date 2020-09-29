// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PhysicsHelpers.h"
#include "SDTCollectible.h"
#include "SDTUtils.h"
#include "SoftDesignTrainingCharacter.h"
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
	const float SPHERE_RADIUS = 1000.f;
	const float CAPSULE_RADIUS = 100.f;

    float _speed = 0.0f;
	float _visionAngle = PI / 3.0f;
    float _yaw = 1.0f;
    FVector _directionGlob;
	float _trainingTimer = 0.0f;
    ASDTCollectible* _powerUp;
	enum PawnState { WANDERING, ROTATING, PICKING_POWERUP, CHASING };
	PawnState _currentState = WANDERING;
	ASoftDesignTrainingCharacter* _agent;

	void Wandering(float deltaTime, APawn* pawn, UWorld* world);
	void Rotating(APawn* pawn, float deltaTime);

	void CalculateSpeed(APawn* pawn, float deltaTime);
	void SetSpeedVector(APawn* pawn, FVector dir);

	FVector GetNextDirection(APawn* pawn, UWorld* world);
	FVector GetNextDirectionParallelToWorld(FVector direction);
	void RotatePawn(APawn* pawn, FRotator rotation);
	FRotator GetRotatorFromDirection(APawn* pawn, FVector newDir);

	void LocateObjects(float deltaTime, APawn* pawn, UWorld* world);
	bool LocateDeathTrap(APawn* pawn, UWorld* world);
	bool LocateDeathTrapOnEachSide(APawn* pawn, UWorld* world, FVector direction);

	bool LocatePowerUp(APawn* pawn, UWorld* world);
	bool LocatePlayer(float deltaTime, APawn* pawn, UWorld* world);

	void PickUpPowerUp(float deltaTime, APawn* pawn, UWorld* world);
	void ChasingPlayer(FVector playerLocation, float deltaTime, APawn* pawn, UWorld* world);

	PhysicsHelpers GetPhysicsHelpers();

	bool IsInsideCone(APawn * pawn, AActor * targetActor) const;

	bool RayCast(APawn* pawn, UWorld* world, const FVector& start, const FVector& end);

	void ValidateExposedParams();
	void ValidateDetectionDist();
	void ValidateMaxSpeed();
	void ValidateAcceleration();

	void DisplayAutomaticTest();

public:
	//Constant used to scale the pawn's max speed. It has to be between 0.5f and 1.0f.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float _maxSpeed = 1.0f;

	//Constant used to determine the distance in which the pawn is able to detect walls in front of him. It has to be between 50.0f and 300.0f.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float _detectionDistance = 150.0f;

	virtual void BeginPlay() override;

    virtual void Tick(float deltaTime) override;

	void OnPawnDeath();
};
