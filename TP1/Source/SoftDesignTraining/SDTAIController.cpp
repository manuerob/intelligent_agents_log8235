// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"

void ASDTAIController::Tick(float deltaTime) {
    Accelerate();
    SetSpeedVector(FVector(1.0f, 0.0f, 0.0f));
}

void ASDTAIController::Accelerate() {
    if (_scale < _maxScale) {
        _scale += _acceleration;
    }
}

void ASDTAIController::SetSpeedVector(FVector dir) {
    APawn* const pawn = GetPawn();

    if (pawn) {
        pawn->AddMovementInput(dir.GetSafeNormal(), _scale);
    }
}

bool ASDTAIController::ChangedDirection(FVector currentDir, FVector newDir) {
    return currentDir.GetSafeNormal().Equals(newDir.GetSafeNormal());
}
