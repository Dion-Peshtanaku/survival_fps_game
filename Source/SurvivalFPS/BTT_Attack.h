// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Enemy.h"
#include "BTT_Attack.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALFPS_API UBTT_Attack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTT_Attack(FObjectInitializer const& a_pObjectInit);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& a_pTreeComp, uint8* a_pNodeMem);
	bool AttackMontageFinished(AEnemy* a_pEnemy);
};
