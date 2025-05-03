// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "IsPlayerInRange.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALFPS_API UIsPlayerInRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	// Constructor
	UIsPlayerInRange();

	// Method for when the service is called
	void OnBecomeRelevant(UBehaviorTreeComponent& a_pTreeComp, uint8* a_pNodeMem);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float m_fRange = 100.0f;
};
