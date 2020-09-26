// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"

bool equalFloats(float a, float b) {
	return fabs(a - b) < 0.001;
}

void ASDTAIController::Tick(float deltaTime) 
{
    APawn* const pawn = GetPawn();
    UWorld* const world = GetWorld();

	_time += deltaTime;

	switch (_currentState) 
	{
		case WANDERING:
			Wandering(deltaTime, pawn, world);
			LocateObjects(pawn, world);
			break;

		case ROTATING:
			Rotating(pawn);
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

void ASDTAIController::Wandering(float deltaTime, APawn* pawn, UWorld* world) 
{
	FVector pawnPosition = pawn->GetActorLocation();

	if (RayCast(pawn, world, pawnPosition, pawnPosition + _detectionDistance * pawn->GetActorForwardVector()))
	{
		_directionGlob = GetNextDirection(pawn, world);
		_currentState = ROTATING;
		_speed /= 2;
		_time = _speed / _a;
	}

	CalculateSpeed(pawn);
	SetSpeedVector(pawn, pawn->GetActorForwardVector());
}

void ASDTAIController::Rotating(APawn* pawn) {
	float angleToRotate = std::acos(FVector::DotProduct(pawn->GetActorForwardVector().GetSafeNormal(), _directionGlob.GetSafeNormal()));
	if (!equalFloats(angleToRotate, 0.0f)) {
		pawn->AddActorWorldRotation(FRotator(0, angleToRotate * _yaw * _speed * 25.0f, 0));
	}
	else {
		_currentState = WANDERING;
	}

	CalculateSpeed(pawn);
	SetSpeedVector(pawn, pawn->GetActorForwardVector());
}

void ASDTAIController::CalculateSpeed(APawn* pawn) 
{
    _speed = _time * _a;

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
	FVector nextDir;
	if (leftRayCast && rightRayCast) {
		if (std::rand() % 2) {
			nextDir = pawnRightVector;
			_yaw = 1.0f;
		}
		else {
			nextDir = pawnRightVector * REVERSE_DIR;
			_yaw = -1.0f;
		}
	}
	else if (leftRayCast) {
		nextDir = pawnRightVector * REVERSE_DIR;
		_yaw = -1.0f;
	}
	else if (rightRayCast) {
		nextDir = pawnRightVector;
		_yaw = 1.0f;
	}
	else {
		nextDir = pawn->GetActorForwardVector() * REVERSE_DIR;
	}

	return GetNextDirectionParallelToWorld(nextDir);
}

FVector ASDTAIController::GetNextDirectionParallelToWorld(FVector direction) {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	if (fabs(direction.X) > fabs(direction.Y)) {
		x = direction.X > 0 ? 1.0f : -1.0f;
	}
	else {
		y = direction.Y > 0 ? 1.0f : -1.0f;
	}

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

void ASDTAIController::LocateObjects(APawn* pawn, UWorld* world) 
{
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

void ASDTAIController::PickUpPowerUp(float deltaTime, APawn* pawn, UWorld* world) 
{
	FVector currentPawnPos = pawn->GetActorLocation();

	FHitResult hit;

	FCollisionObjectQueryParams coqp = FCollisionObjectQueryParams();
	coqp.AddObjectTypesToQuery(COLLISION_COLLECTIBLE);

	FVector toTarget = (_powerUp->GetActorLocation() - pawn->GetTargetLocation())*FVector(1.f, 1.f, 0.f);

	if (world->LineTraceSingleByObjectType(hit, currentPawnPos, _powerUp->GetActorLocation(), coqp)) 
	{
		RotatePawn(pawn, GetRotatorFromDirection(pawn, toTarget));
		//_speed /= 2;
		//_time = _speed / _a;
	}

	CalculateSpeed(pawn);
	SetSpeedVector(pawn, pawn->GetActorForwardVector());

	TArray<AActor*> overlappingActors;
	pawn->GetOverlappingActors(overlappingActors);	

	if (_powerUp->IsOnCooldown()) {
		
		/*for (AActor* actor : overlappingActors) 
		{
			USceneComponent* scene = actor->GetRootComponent();
			if (scene->GetCollisionObjectType() == COLLISION_COLLECTIBLE) 
			{
				UPrimitiveComponent* primComp = static_cast<UPrimitiveComponent*>(pawn->GetRootComponent());
				_pawnMaterial = static_cast<UMeshComponent*>(primComp);

				if(_pawnMaterial == nullptr)
					_pawnMaterial->SetMaterial(0, _poweredUpMaterial);

				world->GetTimerManager().SetTimer(_powerUpTimer, this, &ASDTAIController::OnPowerUpDone, _powerUpDuration, false);
			}

			UE_LOG(LogTemp, Log, TEXT("%s"), *actor->GetName());
		}*/

		_currentState = WANDERING;
	}

	if (LocateDeathTrap(pawn, world))
		_currentState = WANDERING;
}

//void ASDTAIController::OnPowerUpDone() 
//{
//	_pawnMaterial->SetMaterial(0, nullptr);
//	GetWorld()->GetTimerManager().ClearTimer(_powerUpTimer);
//}

bool ASDTAIController::LocateDeathTrap(APawn* pawn, UWorld* world) 
{
	PhysicsHelpers physicsHelpers = GetPhysicsHelpers();

	FHitResult outResult;

	FCollisionObjectQueryParams objectQueryParams; // All objects

	objectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel3);

	bool isDeathTrapFound = world->LineTraceSingleByObjectType(outResult, pawn->GetActorLocation(), pawn->GetActorLocation() + pawn->GetActorForwardVector() * 100.f + FVector(0.f, 0.f, -100.f), objectQueryParams);

	if (isDeathTrapFound) 
	{
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

PhysicsHelpers ASDTAIController::GetPhysicsHelpers() 
{
    UWorld* world = GetWorld();
    return PhysicsHelpers(world);
}