// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToPlayer.h"
#include "../SDTAIController.h"

EBTNodeResult::Type UMoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {
		aiController->MoveToPlayer();

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}