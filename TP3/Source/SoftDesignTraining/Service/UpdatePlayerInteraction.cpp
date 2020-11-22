// Fill out your copyright notice in the Description page of Project Settings.


#include "UpdatePlayerInteraction.h"
#include "../SDTAIController.h"

// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"


void UUpdatePlayerInteraction::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {

		aiController->UpdatePlayerInteraction(DeltaSeconds);
	}
}

