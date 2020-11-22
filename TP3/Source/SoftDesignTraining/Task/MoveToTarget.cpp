// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToTarget.h"
#include "../SDTAIController.h"

#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

EBTNodeResult::Type UMoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {

		aiController->MoveToPlayer();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
