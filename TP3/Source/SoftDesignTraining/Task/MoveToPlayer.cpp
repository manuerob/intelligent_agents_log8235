// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToPlayer.h"
#include "../SDTAIController.h"
#include "DrawDebugHelpers.h"

#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

EBTNodeResult::Type UMoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {
		aiController->MoveToPlayer();

		DrawDebugSphere(GetWorld(), aiController->GetPawnLocation() + FVector(0.f, 0.f, 100.f), 20.f, 32, FColor::Green, false, 0.035f);

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}