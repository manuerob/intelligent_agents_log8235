// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToPowerUp.h"
#include "../SDTAIController.h"

#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

EBTNodeResult::Type UMoveToPowerUp::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {
		aiController->MoveToCollectible();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
