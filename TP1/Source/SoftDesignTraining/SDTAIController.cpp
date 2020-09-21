// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"

void ASDTAIController::Tick(float deltaTime) {
    APawn* const pawn = GetPawn();
    UWorld* const world = GetWorld();

	switch (_currentState) {
		case WANDERING:
			Wandering(deltaTime, pawn, world);
			LocateObjects(pawn, world);
			break;
		case PICKING_POWERUP:
			PickUpPowerUp(deltaTime, pawn, world);
			break;
		case CHASING:
			break;
		default:
			break;
	}

}

void ASDTAIController::Wandering(float deltaTime, APawn* pawn, UWorld* world) {
	FVector currentPawnPos = pawn->GetActorLocation();

	_time += deltaTime;

	if (RayCast(pawn, world, currentPawnPos, currentPawnPos + _detectionDistance * pawn->GetActorForwardVector())) {
		RotatePawn(pawn, GetRotatorFromDirection(pawn, GetNextDirection(pawn, world)));
		_speed /= 2;
		_time = _speed / _a;
	}
	CalculateSpeed(pawn);
	SetSpeedVector(pawn, pawn->GetActorForwardVector());
}


void ASDTAIController::CalculateSpeed(APawn* pawn) {
    _speed = _time * _a;

    if (_speed > _maxSpeed) {
        _speed = _maxSpeed;
    }
}

void ASDTAIController::SetSpeedVector(APawn* pawn, FVector dir) {
    if (pawn) {
        pawn->AddMovementInput(dir.GetSafeNormal(), _speed);
    }
}

FVector ASDTAIController::GetNextDirection(APawn* pawn, UWorld* world) {
    FVector pawnRightVector = pawn->GetActorRightVector();
    bool leftRayCast = !RayCast(pawn, world, pawn->GetActorLocation(), pawn->GetActorLocation() + _detectionDistance * pawnRightVector * REVERSE_DIR);
    bool rightRayCast = !RayCast(pawn, world, pawn->GetActorLocation(), pawn->GetActorLocation() + _detectionDistance * pawnRightVector);
    FVector nextDir;
    if (leftRayCast && rightRayCast) {
        nextDir = std::rand() % 2 ? pawnRightVector : pawnRightVector * REVERSE_DIR;
    }
    else if(leftRayCast) {
        nextDir = pawnRightVector * REVERSE_DIR;
    }
    else if (rightRayCast) {
        nextDir = pawnRightVector;
    }
    else {
        nextDir = pawn->GetActorForwardVector() * REVERSE_DIR;
    }

    return nextDir;
}

void ASDTAIController::RotatePawn(APawn* pawn, FRotator rotation) {
    pawn->AddActorWorldRotation(rotation);
}

FRotator ASDTAIController::GetRotatorFromDirection(APawn* pawn, FVector newDir) {
    return newDir.ToOrientationRotator() - pawn->GetActorForwardVector().ToOrientationRotator();
}

bool ASDTAIController::RayCast(APawn* pawn, UWorld* world, const FVector& start, const FVector& end) {
    FHitResult hit;
    return world->LineTraceSingleByObjectType(hit, start, end, FCollisionObjectQueryParams().AllStaticObjects);
}

void ASDTAIController::LocateObjects(APawn* pawn, UWorld* world) {
    
    PhysicsHelpers physicsHelpers = GetPhysicsHelpers();

	LocateDeathTrap(pawn, world);

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
				if (hit.GetComponent()->GetCollisionObjectType() == COLLISION_COLLECTIBLE) 
				{
					ASDTCollectible* collectible = static_cast<ASDTCollectible*>(hit.GetActor());
					if (!collectible->IsOnCooldown())
					{
						_currentState = PICKING_POWERUP;
						_powerUp = collectible;
					}
				}
			}
		}

	}
}

void ASDTAIController::PickUpPowerUp(float deltaTime, APawn* pawn, UWorld* world) {

	FVector currentPawnPos = pawn->GetActorLocation();

	_time += deltaTime;

	FHitResult hit;

	FCollisionObjectQueryParams coqp = FCollisionObjectQueryParams();
	coqp.AddObjectTypesToQuery(COLLISION_COLLECTIBLE);

	FVector toTarget = (_powerUp->GetActorLocation() - pawn->GetTargetLocation())*FVector(1.f, 1.f, 0.f);

	if (world->LineTraceSingleByObjectType(hit, currentPawnPos, _powerUp->GetActorLocation(), coqp)) {
		RotatePawn(pawn, GetRotatorFromDirection(pawn, toTarget));
		//_speed /= 2;
		//_time = _speed / _a;
	}
	CalculateSpeed(pawn);
	SetSpeedVector(pawn, pawn->GetActorForwardVector());
	
	if (toTarget.Size() < (FMath::Min(_maxSpeed, _speed) * deltaTime * pawn->GetVelocity()).Size()) {
		UE_LOG(LogTemp, Log, TEXT("GOT THIS THING"));
	}

	if (_powerUp->IsOnCooldown()) {
		UE_LOG(LogTemp, Log, TEXT("%f"), toTarget.Size());
		UE_LOG(LogTemp, Log, TEXT("%f"), (FMath::Min(_maxSpeed, _speed) * deltaTime * pawn->GetVelocity()).Size());
		_currentState = WANDERING;
	}

	if (LocateDeathTrap(pawn, world)) {
		_currentState = WANDERING;
	}
	
}

bool ASDTAIController::LocateDeathTrap(APawn* pawn, UWorld* world) {
	PhysicsHelpers physicsHelpers = GetPhysicsHelpers();

	FHitResult outResult;

	FCollisionObjectQueryParams objectQueryParams; // All objects

	objectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel3);

	bool isDeathTrapFound = world->LineTraceSingleByObjectType(outResult, pawn->GetActorLocation(), pawn->GetActorLocation() + pawn->GetActorForwardVector() * 100.f + FVector(0.f, 0.f, -100.f), objectQueryParams);

	if (isDeathTrapFound) {
		RotatePawn(pawn, GetRotatorFromDirection(pawn, -pawn->GetActorForwardVector()));
		return true;
	} 
	return false;
}

bool ASDTAIController::IsInsideCone(APawn * pawn, AActor * targetActor) const
{
	FVector const toTarget = targetActor->GetActorLocation() - pawn->GetActorLocation();
	FVector const pawnForward = pawn->GetActorForwardVector();
	return std::abs(std::acos(FVector::DotProduct(pawnForward.GetSafeNormal(), toTarget.GetSafeNormal()))) < _visionAngle;
}

PhysicsHelpers ASDTAIController::GetPhysicsHelpers() {
    UWorld* world = GetWorld();
    return PhysicsHelpers(world);
}