// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SDTBaseAIController.h"
#include "SoftDesignTrainingMainCharacter.h"
#include "SDTAIController.generated.h"

/**
 *
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public ASDTBaseAIController
{
    GENERATED_BODY()

public:
    ASDTAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float m_DetectionCapsuleHalfLength = 500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float m_DetectionCapsuleRadius = 250.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float m_DetectionCapsuleForwardStartingOffset = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        UCurveFloat* JumpCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float JumpApexHeight = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float JumpSpeed = 1.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
        bool AtJumpSegment = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
        bool InAir = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
        bool Landing = false;
	

public:
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
    void AIStateInterrupted();
    virtual void BeginPlay() override;
	bool isPlayer = false;

protected:
    void OnMoveToTarget();
    void GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit);
    void UpdatePlayerInteraction(float deltaTime);

private:
	AActor* _targetActor;
	ASoftDesignTrainingMainCharacter* _player = nullptr;
	FVector _actorPos;
	enum ActorType { COLLECTIBLE, FLEE_LOCATION };

    virtual void GoToBestTarget(float deltaTime) override;
    virtual void ChooseBehavior(float deltaTime) override;
    virtual void ShowNavigationPath() override;
	virtual void GoToSelectedTarget(float deltaTime) override;
    AActor* GetClosestActor(APawn* pawn, TArray<AActor*> actors, ActorType actorType);
    bool VerifyCollectibleCooldown(TArray<AActor*> collectibles, FNavPathPoint collectionLocation);
	virtual bool GetPlayer() override;

    // Jump
    float baseHeight = 0.0f;
    float currentJumpTime = 0.0f;
    float totalJumpTime = 0.0f;
    FVector jumpDirection;
    float maxDeltaHeight = 0.0f;
    FVector startingJumpPos;
    FVector endingJumpPos;

    void Jump(float deltaTime);
    void PrepareJump(float deltaTime);
    void UpdateJump(float deltaTime);
};
