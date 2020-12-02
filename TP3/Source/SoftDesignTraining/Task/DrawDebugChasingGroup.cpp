// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawDebugChasingGroup.h"
#include "../SDTAIController.h"
#include "DrawDebugHelpers.h"

EBTNodeResult::Type UDrawDebugChasingGroup::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {

		DrawDebugSphere(GetWorld(), aiController->GetPawnLocation() + FVector(0.f, 0.f, 100.f), 20.f, 32, FColor::Green, false, 0.035f);

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}