// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingMainCharacter.h"
#include "DrawDebugHelpers.h"

bool equalFloats(float a, float b) 
{
	return fabs(a - b) < 0.001;
}

void ASDTAIController::Tick(float deltaTime) 
{
    APawn* const pawn = GetPawn();
    UWorld* const world = GetWorld();

	ValidateExposedParams();

	switch (_currentState) 
	{
		case WANDERING:
			UE_LOG(LogTemp, Log, TEXT("WANDERING"));
			LocateObjects(deltaTime, pawn, world);
			Wandering(deltaTime, pawn, world);
			break;

		case ROTATING:
			UE_LOG(LogTemp, Log, TEXT("ROTATING"));
			Rotating(pawn, deltaTime);
			break;

		case PICKING_POWERUP:
			UE_LOG(LogTemp, Log, TEXT("POWERUP"));
			PickUpPowerUp(deltaTime, pawn, world);
			LocatePlayer(deltaTime, pawn, world);
			break;

		case CHASING:
			UE_LOG(LogTemp, Log, TEXT("Chasing"));
			LocatePlayer(deltaTime, pawn, world);
			break;

		default:
			break;
	}
}

void ASDTAIController::Wandering(float deltaTime, APawn* pawn, UWorld* world) 
{
	FVector pawnPosition = pawn->GetActorLocation();

	if (RayCast(pawn, world, pawnPosition, pawnPosition + _detectionDistance * pawn->GetActorForwardVector()))
	{
		_directionGlob = GetNextDirection(pawn, world);
		_currentState = ROTATING;
		_speed /= 2;
	}

	CalculateSpeed(pawn, deltaTime);
	SetSpeedVector(pawn, pawn->GetActorForwardVector());
}

void ASDTAIController::Rotating(APawn* pawn, float deltaTime) 
{
	float angleToRotate = std::acos(FVector::DotProduct(pawn->GetActorForwardVector().GetSafeNormal(), _directionGlob.GetSafeNormal()));

	if (!equalFloats(angleToRotate, 0.0f))
		pawn->AddActorWorldRotation(FRotator(0, angleToRotate * _yaw * _speed * 25.0f, 0));
	else 
		_currentState = WANDERING;

	CalculateSpeed(pawn, deltaTime);
	SetSpeedVector(pawn, pawn->GetActorForwardVector());
}

void ASDTAIController::CalculateSpeed(APawn* pawn, float deltaTime) 
{
	_speed += deltaTime * _a;

    if (_speed > _maxSpeed)
        _speed = _maxSpeed;
}

void ASDTAIController::SetSpeedVector(APawn* pawn, FVector dir) 
{
    if (pawn)
        pawn->AddMovementInput(dir.GetSafeNormal(), _speed);
}

FVector ASDTAIController::GetNextDirection(APawn* pawn, UWorld* world) 
{
	FVector pawnRightVector = pawn->GetActorRightVector();
	bool leftRayCast = !RayCast(pawn, world, pawn->GetActorLocation(), pawn->GetActorLocation() + 350.0f * pawnRightVector * REVERSE_DIR);
	bool rightRayCast = !RayCast(pawn, world, pawn->GetActorLocation(), pawn->GetActorLocation() + 350.0f * pawnRightVector);
	bool rightDeathTrap = LocateDeathTrap(pawn, world, pawnRightVector);
	bool leftDeathTrap = LocateDeathTrap(pawn, world, pawnRightVector * REVERSE_DIR);
	bool isLeftClear = !leftDeathTrap && leftRayCast;
	bool isRightClear = !rightDeathTrap && rightRayCast;
	FVector nextDir;
	
	if (isLeftClear && isRightClear)
	{
		if (std::rand() % 2) 
		{
			nextDir = pawnRightVector;
			_yaw = 1.0f;
		}
		else 
		{
			nextDir = pawnRightVector * REVERSE_DIR;
			_yaw = -1.0f;
		}
	}
	else if (isLeftClear)
	{
		nextDir = pawnRightVector * REVERSE_DIR;
		_yaw = -1.0f;
	}
	else if (isRightClear)
	{
		nextDir = pawnRightVector;
		_yaw = 1.0f;
	}
	else 
	{
		nextDir = pawn->GetActorForwardVector() * REVERSE_DIR;
	}

	return GetNextDirectionParallelToWorld(nextDir);
}

FVector ASDTAIController::GetNextDirectionParallelToWorld(FVector direction) 
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	if (fabs(direction.X) > fabs(direction.Y))
		x = direction.X > 0 ? 1.0f : -1.0f;
	else
		y = direction.Y > 0 ? 1.0f : -1.0f;

	return FVector(x, y, z);
}

void ASDTAIController::RotatePawn(APawn* pawn, FRotator rotation) 
{
    pawn->AddActorWorldRotation(rotation);
}

FRotator ASDTAIController::GetRotatorFromDirection(APawn* pawn, FVector newDir) 
{
    return newDir.ToOrientationRotator() - pawn->GetActorForwardVector().ToOrientationRotator();
}

bool ASDTAIController::RayCast(APawn* pawn, UWorld* world, const FVector& start, const FVector& end) 
{
    FHitResult hit;
    return world->LineTraceSingleByObjectType(hit, start, end, FCollisionObjectQueryParams().AllStaticObjects);
}

void ASDTAIController::LocateObjects(float deltaTime, APawn* pawn, UWorld* world)
{
	LocateDeathTrap(pawn, world);

	LocatePlayer(deltaTime, pawn, world);

	LocatePowerUp(pawn, world);
}

bool ASDTAIController::LocatePlayer(float deltaTime, APawn* pawn, UWorld* world) 
{
	PhysicsHelpers physicsHelpers = GetPhysicsHelpers();

	TArray<FOverlapResult> outResults;

	physicsHelpers.SphereOverlap(pawn->GetActorLocation() + pawn->GetActorForwardVector() * 1000.f, 1000.f, outResults, true, COLLISION_PLAYER);

	for (FOverlapResult outResult : outResults)
	{
		if (IsInsideCone(GetPawn(), outResult.GetActor()))
		{
			FHitResult hit;
			AActor* actor = outResult.GetActor();

			FCollisionObjectQueryParams coqp = FCollisionObjectQueryParams();
			coqp.AddObjectTypesToQuery(COLLISION_PLAYER);
			coqp.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);

			if (world->LineTraceSingleByObjectType(hit, pawn->GetActorLocation(), actor->GetActorLocation(), coqp))
			{
				if (hit.GetComponent()->GetCollisionObjectType() == COLLISION_PLAYER) {
					ASoftDesignTrainingMainCharacter* player = static_cast<ASoftDesignTrainingMainCharacter*>(hit.GetActor());
					if (!player->IsPoweredUp()) 
					{
						_currentState = CHASING;
						_powerUp = nullptr;
						ChasingPlayer(player->GetActorLocation(), deltaTime, pawn, world);

						return true;
					}
					else 
					{
						FVector escapeDir = GetNextDirection(pawn, world);
						RotatePawn(pawn, GetRotatorFromDirection(pawn, escapeDir));
						_currentState = WANDERING;
					}
				}
				else 
				{
					_currentState = WANDERING;
				}
			}
			else
			{
				_currentState = WANDERING;
			}
		}
		else
		{
			_currentState = WANDERING;
		}
	}

	if (_currentState == CHASING)
		_currentState = WANDERING;

	return false;
}

void ASDTAIController::ChasingPlayer(FVector playerLocation, float deltaTime, APawn* pawn, UWorld* world) 
{
	FVector toTarget = (playerLocation - pawn->GetTargetLocation())*FVector(1.f, 1.f, 0.f);

	RotatePawn(pawn, GetRotatorFromDirection(pawn, toTarget));

	CalculateSpeed(pawn, deltaTime);
	SetSpeedVector(pawn, pawn->GetActorForwardVector());

	TArray<AActor*> overlappingActors;
	pawn->GetOverlappingActors(overlappingActors);

	//for (AActor* actor : overlappingActors) 
	//{
	//	USceneComponent* scene = actor->GetRootComponent();
	//	if (scene->GetCollisionObjectType() == COLLISION_PLAYER) {
	//		UE_LOG(LogTemp, Log, TEXT("afdg"));
	//	}
	//}

	if (LocateDeathTrap(pawn, world))
		_currentState = WANDERING;
}

void ASDTAIController::PickUpPowerUp(float deltaTime, APawn* pawn, UWorld* world) 
{
	FVector toTarget = (_powerUp->GetActorLocation() - pawn->GetTargetLocation())*FVector(1.f, 1.f, 0.f);

	RotatePawn(pawn, GetRotatorFromDirection(pawn, toTarget));

	CalculateSpeed(pawn, deltaTime);
	SetSpeedVector(pawn, pawn->GetActorForwardVector());

	if (_powerUp->IsOnCooldown()) 
	{
		_currentState = WANDERING;
		_powerUp = nullptr;
	}

	if (LocateDeathTrap(pawn, world)) 
	{
		_currentState = WANDERING;
		_powerUp = nullptr;
	}	
}

bool ASDTAIController::LocatePowerUp(APawn* pawn, UWorld* world)
{
	PhysicsHelpers physicsHelpers = GetPhysicsHelpers();

	TArray<FOverlapResult> outResults;

	physicsHelpers.SphereOverlap(pawn->GetActorLocation() + pawn->GetActorForwardVector() * 1000.f, 1000.f, outResults, true, COLLISION_COLLECTIBLE);

	for (FOverlapResult outResult : outResults)
	{
		if (IsInsideCone(GetPawn(), outResult.GetActor()))
		{
			FHitResult hit;
			AActor* actor = outResult.GetActor();

			FCollisionObjectQueryParams coqp = FCollisionObjectQueryParams();
			coqp.AddObjectTypesToQuery(COLLISION_COLLECTIBLE);
			coqp.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);

			if (world->LineTraceSingleByObjectType(hit, pawn->GetActorLocation(), actor->GetActorLocation(), coqp))
			{
				if (hit.GetComponent()->GetCollisionObjectType() == COLLISION_COLLECTIBLE && !_powerUp)
				{
					ASDTCollectible* collectible = static_cast<ASDTCollectible*>(hit.GetActor());
					if (!collectible->IsOnCooldown())
					{
						_currentState = PICKING_POWERUP;
						_powerUp = collectible;

						return true;
					}
				}
			}
		}
	}

	return false;
}

bool ASDTAIController::LocateDeathTrap(APawn* pawn, UWorld* world) 
{
	PhysicsHelpers physicsHelpers = GetPhysicsHelpers();

	FHitResult outResult;

	FCollisionObjectQueryParams objectQueryParams;

	objectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel3);

	bool isDeathTrapFound = world->LineTraceSingleByObjectType(outResult, pawn->GetActorLocation(), pawn->GetActorLocation() + pawn->GetActorForwardVector() * 150.f + FVector(0.f, 0.f, -100.f), objectQueryParams);

	if (isDeathTrapFound) 
	{
		FVector escapeDir = GetNextDirection(pawn, world);
		RotatePawn(pawn, GetRotatorFromDirection(pawn, escapeDir));
		SetSpeedVector(pawn, pawn->GetActorForwardVector());
		return true;
	} 

	return false;
}

bool ASDTAIController::LocateDeathTrap(APawn* pawn, UWorld* world, FVector direction) {
	PhysicsHelpers physicsHelpers = GetPhysicsHelpers();

	FHitResult outResult;

	FCollisionObjectQueryParams objectQueryParams;

	objectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel3);
	bool hitDetected = world->LineTraceSingleByObjectType(outResult, pawn->GetActorLocation(), pawn->GetActorLocation() + direction * 200.f + FVector(0.f, 0.f, -100.f), objectQueryParams);

	return (hitDetected && outResult.Distance < 200.0f);
}

bool ASDTAIController::IsInsideCone(APawn * pawn, AActor * targetActor) const
{
	FVector const toTarget = targetActor->GetActorLocation() - pawn->GetActorLocation();
	FVector const pawnForward = pawn->GetActorForwardVector();
	return std::abs(std::acos(FVector::DotProduct(pawnForward.GetSafeNormal(), toTarget.GetSafeNormal()))) < _visionAngle;
}

PhysicsHelpers ASDTAIController::GetPhysicsHelpers() 
{
    UWorld* world = GetWorld();
    return PhysicsHelpers(world);
}

void ASDTAIController::OnPawnDeath() 
{
	_powerUp = nullptr;
	_currentState = WANDERING;
}

void ASDTAIController::ValidateExposedParams() {
	ValidateDetectionDist();
	ValidateMaxSpeed();
	ValidateAcceleration();
}

void ASDTAIController::ValidateDetectionDist() {
	if (_detectionDistance < 50.0f) {
		_detectionDistance = 50.0f;
	} else if (_detectionDistance > 300.0f) {
		_detectionDistance = 300.0f;
	}
}

void ASDTAIController::ValidateMaxSpeed() {
	if (_maxSpeed < 0.5f) {
		_maxSpeed = 0.5f;
	} else if (_maxSpeed > 1.0f) {
		_maxSpeed = 1.0f;
	}
}

void ASDTAIController::ValidateAcceleration() {
	if (_a < 0.01f) {
		_a = 0.01f;
	} else if (_a > 1.0f) {
		_a = 1.0f;
	}
}