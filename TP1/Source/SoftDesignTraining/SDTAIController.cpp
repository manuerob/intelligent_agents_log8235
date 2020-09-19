// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"

void ASDTAIController::Tick(float deltaTime) {
    APawn* const pawn = GetPawn();
    UWorld* const world = GetWorld();
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
    UE_LOG(LogTemp, Log, TEXT(" %f, %f, %f, %f, %f"), _speed, _a, _maxSpeed, _detectionDistance, _time);

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