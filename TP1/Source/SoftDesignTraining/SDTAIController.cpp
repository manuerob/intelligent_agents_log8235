// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"

void ASDTAIController::Tick(float deltaTime) {
    APawn* const pawn = GetPawn();
    UWorld* const world = GetWorld();
    FVector currentPawnPos = pawn->GetActorLocation();

    if (RayCast(pawn, world, currentPawnPos, currentPawnPos + _detectionDistance * pawn->GetActorForwardVector())) {
        RotatePawn(pawn, GetRotatorFromDirection(pawn, GetNextDirection(pawn, world)));
        _speed = 0.2;
    }
    Accelerate(pawn);
    SetSpeedVector(pawn, pawn->GetActorForwardVector());

}

void ASDTAIController::Accelerate(APawn* pawn) {
    if (_speed < _maxSpeed) {
        _speed += _acceleration;
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