// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BehaviorTree/Task/BTTask_MoveToTarget.h"
#include "SoftDesignTraining.h"
#include "AI/SoftDesignAIController.h"
#include "SoftDesignTrainingCharacter.h"
#include "AIBase.h"

#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


EBTNodeResult::Type UBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent())
    {
        if (ASoftDesignAIController* aiController = Cast<ASoftDesignAIController>(OwnerComp.GetAIOwner()))
        {
            FVector targetPosition = FVector::ZeroVector;
            if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
            {
                if (AActor* targetActor = Cast<AActor>(MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID())))
                {
                    targetPosition = targetActor->GetActorLocation();
                }
            }
            else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
            {
                uint8 bbKey = BlackboardKey.GetSelectedKeyID();
                targetPosition = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(bbKey);
                
                //Agent is moving to target
                if (bbKey == aiController->GetTargetPosBBKeyID())
                {
                    if (AAIBase* aiBase = Cast<AAIBase>(aiController->GetCharacter()))
                    {
                        aiBase->SetCurrentPatrolDestination(FVector::ZeroVector);
                        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetCurrentPatrolDestinationKeyID(), FVector::ZeroVector);
                    }
                }
            }
            else
            {
                targetPosition = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPosBBKeyID());
            }

            UAIBlueprintHelperLibrary::SimpleMoveToLocation(aiController, targetPosition);

            return EBTNodeResult::Succeeded;
        }
    }    

    return EBTNodeResult::Failed;
}




