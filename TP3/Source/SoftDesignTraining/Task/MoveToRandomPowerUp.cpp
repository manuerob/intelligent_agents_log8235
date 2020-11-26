#include "MoveToRandomPowerUp.h"
#include "../SDTAIController.h"

#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

EBTNodeResult::Type UMoveToRandomPowerUp::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {

		//aiController->MoveToRandomCollectible();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}