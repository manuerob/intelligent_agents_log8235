// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingMainCharacter.h"
#include <Runtime\Engine\Classes\Kismet\KismetMathLibrary.h>
#include "DrawDebugHelpers.h"

bool equalFloats(float a, float b) 
{
	return fabs(a - b) < 0.001;
}

void ASDTAIController::BeginPlay() {
	Super::BeginPlay();

	AActor* agentActor = static_cast<AActor*>(GetPawn());
	_agent = static_cast<ASoftDesignTrainingCharacter*>(agentActor);
}

void ASDTAIController::Tick(float deltaTime) 
{

    APawn* const pawn = GetPawn();
    UWorld* const world = GetWorld();
	ValidateExposedParams();

	if (_agent != nullptr && _agent->isRespawn)
			OnPawnDeath();

	_trainingTimer += deltaTime;
	DisplayAutomaticTest();

	switch (_currentState) 
	{
		case WANDERING:
			LocateObjects(deltaTime, pawn, world);
			Wandering(deltaTime, pawn, world);
			break;

		case ROTATING:
			Rotating(pawn, deltaTime);
			LocateObjects(deltaTime, pawn, world);
			break;

		case PICKING_POWERUP:
			PickUpPowerUp(deltaTime, pawn, world);
			LocatePlayer(deltaTime, pawn, world);
			break;

		case CHASING:
			LocatePlayer(deltaTime, pawn, world);
			break;

		default:
			break;
	}
}

//Function called when the pawn is in the WANDERING state. When a wall is detected he is switched to the ROTATING state.
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

//Function called when the pawn is in the ROTATING state. When the rotation is done the pawn is switched to the WANDERING state.
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


//Function used to calculate the new speed of the pawn, that doesn't exceed the maximum speed.
void ASDTAIController::CalculateSpeed(APawn* pawn, float deltaTime) 
{
	_speed += deltaTime * GetCharacter()->GetCharacterMovement()->GetMaxAcceleration();

    if (_speed > _maxSpeed)
        _speed = _maxSpeed;
}

//Function that sets the pawn's speed.
void ASDTAIController::SetSpeedVector(APawn* pawn, FVector dir) 
{
    if (pawn)
        pawn->AddMovementInput(dir.GetSafeNormal(), _speed);
}

//Function that determines the next direction of the pawn while taking into consideration the pawn's surroundings.
FVector ASDTAIController::GetNextDirection(APawn* pawn, UWorld* world) 
{
	FVector pawnRightVector = pawn->GetActorRightVector();
	bool leftRayCast = !RayCast(pawn, world, pawn->GetActorLocation(), pawn->GetActorLocation() + 350.0f * pawnRightVector * REVERSE_DIR);
	bool rightRayCast = !RayCast(pawn, world, pawn->GetActorLocation(), pawn->GetActorLocation() + 350.0f * pawnRightVector);
	bool rightDeathTrap = LocateDeathTrapOnEachSide(pawn, world, pawnRightVector);
	bool leftDeathTrap = LocateDeathTrapOnEachSide(pawn, world, pawnRightVector * REVERSE_DIR);
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

//Function that redirects the pawn to be parrallel to the world axes.
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

//Function that rotates the pawn to the new calculated rotation.
void ASDTAIController::RotatePawn(APawn* pawn, FRotator rotation) 
{
    pawn->AddActorWorldRotation(rotation);
}

//Function that gets the rotator of the pawn for a given direction.
FRotator ASDTAIController::GetRotatorFromDirection(APawn* pawn, FVector newDir) 
{
    return newDir.ToOrientationRotator() - pawn->GetActorForwardVector().ToOrientationRotator();
}

//Function that casts a raycast to detect if a wall is present. 
bool ASDTAIController::RayCast(APawn* pawn, UWorld* world, const FVector& start, const FVector& end) 
{
    FHitResult hit;
    return world->LineTraceSingleByObjectType(hit, start, end, FCollisionObjectQueryParams().AllStaticObjects);
}

//Function that locates death traps, players and power ups.
void ASDTAIController::LocateObjects(float deltaTime, APawn* pawn, UWorld* world)
{
	if (!LocateDeathTrap(pawn, world)) {
		if (!LocatePlayer(deltaTime, pawn, world))
			LocatePowerUp(pawn, world);
	}

}

//Function that locates if the player is in front of the pawn and if the player isn't powered up the pawn is put in the CHASING state.
//If the player is powered up, the pawn is put in the ROTATING state.
//if he is in the CHASING state and doesn't see the player anymore he is put in the WANDERING state.
bool ASDTAIController::LocatePlayer(float deltaTime, APawn* pawn, UWorld* world) 
{
	PhysicsHelpers physicsHelpers = GetPhysicsHelpers();
	TArray<FOverlapResult> outResults;

	physicsHelpers.SphereOverlap(pawn->GetActorLocation() + pawn->GetActorForwardVector() * SPHERE_RADIUS, SPHERE_RADIUS, outResults, false, COLLISION_PLAYER);

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
						_directionGlob = GetNextDirection(pawn, world);
						_currentState = ROTATING;
					}
				}
			}
		}
	}

	if (_currentState == CHASING)
		_currentState = WANDERING;

	return false;
}

//Function called when the pawn is in the CHASING state. Changes the rotation and speed of the pawn to face the player it is chasing.
//If the pawn locates a death trap, he is put in the WANDERING state.
void ASDTAIController::ChasingPlayer(FVector playerLocation, float deltaTime, APawn* pawn, UWorld* world) 
{
	FVector toTarget = (playerLocation - pawn->GetTargetLocation())*FVector(1.f, 1.f, 0.f);

	RotatePawn(pawn, GetRotatorFromDirection(pawn, toTarget));

	CalculateSpeed(pawn, deltaTime);
	SetSpeedVector(pawn, pawn->GetActorForwardVector());

	TArray<AActor*> overlappingActors;
	pawn->GetOverlappingActors(overlappingActors);

	if (LocateDeathTrap(pawn, world))
		_currentState = WANDERING;
}

//Function called when the pawn is in the PICKING_POWERUP state. Changes the rotation and speed of the pawn to face the desired power up.
//If the desired powerup is picked up by another actor, or if the pawn detects a death trap, he is put in the WANDERING state.
void ASDTAIController::PickUpPowerUp(float deltaTime, APawn* pawn, UWorld* world) 
{
	FVector toTarget = (_powerUp->GetActorLocation() - pawn->GetTargetLocation())*FVector(1.f, 1.f, 0.f);

	RotatePawn(pawn, GetRotatorFromDirection(pawn, toTarget));

	CalculateSpeed(pawn, deltaTime);
	SetSpeedVector(pawn, pawn->GetActorForwardVector());

	if (_powerUp->IsOnCooldown() || LocateDeathTrap(pawn, world))
	{
		_currentState = WANDERING;
		_powerUp = nullptr;
	}
}

//Function that locates a powerup in front of the pawn. If he finds one in front of him he is then put in the PICKING_POWERUP state.
bool ASDTAIController::LocatePowerUp(APawn* pawn, UWorld* world)
{
	PhysicsHelpers physicsHelpers = GetPhysicsHelpers();

	TArray<FOverlapResult> outResults;

	physicsHelpers.SphereOverlap(pawn->GetActorLocation() + pawn->GetActorForwardVector() * SPHERE_RADIUS, SPHERE_RADIUS, outResults, false, COLLISION_COLLECTIBLE);

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

//Function that locates death traps, if a death trap is found around the character, a new calculated direction that is perpendicular to the deathtrap is assigned to the pawn.
//If a death trap is located the pawn is put in the WANDERING state.
bool ASDTAIController::LocateDeathTrap(APawn* pawn, UWorld* world) 
{
	PhysicsHelpers physicsHelpers = GetPhysicsHelpers();

	FHitResult outResult;

	FCollisionObjectQueryParams objectQueryParams;

	objectQueryParams.AddObjectTypesToQuery(COLLISION_DEATH_OBJECT);

	bool isDeathTrapFound = world->SweepSingleByObjectType(outResult, pawn->GetActorLocation(), pawn->GetActorLocation() + pawn->GetActorForwardVector() * CAPSULE_RADIUS, FQuat(), objectQueryParams, FCollisionShape::MakeSphere(100.f));

	if (isDeathTrapFound) 
	{
		FVector contactDirection = FVector::CrossProduct(FVector::UpVector, FVector(outResult.ImpactNormal.X, outResult.ImpactNormal.Y, 0.f));
		RotatePawn(pawn, GetRotatorFromDirection(pawn, contactDirection));
		SetSpeedVector(pawn, pawn->GetActorForwardVector());
		_currentState = WANDERING;
		return true;
	} 

	return false;
}

//Function called when trying to determine which side to go to after colliding with a wall.
//Locates death traps in a given direction of the pawn.
bool ASDTAIController::LocateDeathTrapOnEachSide(APawn* pawn, UWorld* world, FVector direction) {
	PhysicsHelpers physicsHelpers = GetPhysicsHelpers();

	FHitResult outResult;

	FCollisionObjectQueryParams objectQueryParams;

	objectQueryParams.AddObjectTypesToQuery(COLLISION_DEATH_OBJECT);
	bool hitDetected = world->LineTraceSingleByObjectType(outResult, pawn->GetActorLocation(), pawn->GetActorLocation() + direction * 200.f + FVector(0.f, 0.f, -100.f), objectQueryParams);

	return (hitDetected && outResult.Distance < 200.0f);
}

//Function that verifies if the target actor is in a cone in front of the pawn.
bool ASDTAIController::IsInsideCone(APawn * pawn, AActor * targetActor) const
{
	FVector const toTarget = targetActor->GetActorLocation() - pawn->GetActorLocation();
	FVector const pawnForward = pawn->GetActorForwardVector();
	return std::abs(std::acos(FVector::DotProduct(pawnForward.GetSafeNormal(), toTarget.GetSafeNormal()))) < _visionAngle;
}

//Function that returns the physics helper.
PhysicsHelpers ASDTAIController::GetPhysicsHelpers() 
{
    UWorld* world = GetWorld();
    return PhysicsHelpers(world);
}

//Function that reinitialise the pawn once dead. 
void ASDTAIController::OnPawnDeath() 
{
	_powerUp = nullptr;
	_currentState = WANDERING;
	_agent->isRespawn = false;
}

//Function that validates the detection distance, the max speed and the acceleration of the pawn.
void ASDTAIController::ValidateExposedParams() {
	ValidateDetectionDist();
	ValidateMaxSpeed();
	ValidateAcceleration();
}

//Function that validates the detection distance of the pawn.
void ASDTAIController::ValidateDetectionDist() {
	if (_detectionDistance < 50.0f) {
		_detectionDistance = 50.0f;
	} else if (_detectionDistance > 300.0f) {
		_detectionDistance = 300.0f;
	}
}

//Function that validates the max speed of the pawn.
void ASDTAIController::ValidateMaxSpeed() {
	if (_maxSpeed < 0.5f) {
		_maxSpeed = 0.5f;
	} else if (_maxSpeed > 1.0f) {
		_maxSpeed = 1.0f;
	}
}

//Function that validates the acceleration of the pawn.
void ASDTAIController::ValidateAcceleration() {
	if (GetCharacter()->GetCharacterMovement()->GetMaxAcceleration() < 150.0f) {
		GetCharacter()->GetCharacterMovement()->MaxAcceleration = 150.0f;
	} else if (GetCharacter()->GetCharacterMovement()->GetMaxAcceleration() > 500.0f) {
		GetCharacter()->GetCharacterMovement()->MaxAcceleration = 500.0f;
	}
}

//Function that displays the autimatic test of the different pawns. This displays the number of pickups and death of a given pawn, as well as a timer since game start.
void ASDTAIController::DisplayAutomaticTest()
{
	FTimespan clockedTime = UKismetMathLibrary::MakeTimespan(0, 0, 0, _trainingTimer, 0);

	FString timeMessage = "Time : " + clockedTime.ToString();
	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Orange, timeMessage, true);

	FString actorPickUpDeathMessage = _agent->GetActorLabel() + " : " + FString::FromInt(_agent->pickUpCount) + " pickups and " + FString::FromInt(_agent->deathCount) + "  deaths";
	GEngine->AddOnScreenDebugMessage(_agent->key + 1, 5.0f, FColor::Magenta, actorPickUpDeathMessage, true);
}