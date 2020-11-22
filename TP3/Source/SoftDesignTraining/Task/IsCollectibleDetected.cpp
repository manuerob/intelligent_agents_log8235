// Fill out your copyright notice in the Description page of Project Settings.


#include "IsCollectibleDetected.h"
#include "../SDTAIController.h"

EBTNodeResult::Type UIsCollectibleDetected::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {

	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {

		return aiController->IsCollectibleDetected() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}
	return EBTNodeResult::Failed;
}

