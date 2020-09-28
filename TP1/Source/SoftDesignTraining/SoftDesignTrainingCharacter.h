// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "SoftDesignTrainingCharacter.generated.h"

UCLASS()
class ASoftDesignTrainingCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASoftDesignTrainingCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	int key;

    virtual void BeginPlay() override;
    virtual void OnCollectPowerUp() {};

	int deathCount = 0;
	int pickUpCount = 0;
	bool isRespawn = false;

protected:
    UFUNCTION()
    virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    FVector m_StartingPosition;

	FRotator m_StartingRotation;

};

