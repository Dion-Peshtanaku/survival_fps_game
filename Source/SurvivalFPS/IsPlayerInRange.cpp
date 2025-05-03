// Fill out your copyright notice in the Description page of Project Settings.


#include "IsPlayerInRange.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "C:\Users\User\Documents\Unreal Projects\SurvivalFPS\Intermediate\ProjectFiles\EnemyKeys.h"

UIsPlayerInRange::UIsPlayerInRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Is Player In Range");
}

void UIsPlayerInRange::OnBecomeRelevant(UBehaviorTreeComponent& a_pTreeComp, uint8* a_pNodeMem)
{
	// Super call
	Super::OnBecomeRelevant(a_pTreeComp, a_pNodeMem);

	// Get AI Controller and Pawn
	AEnemy_Controller* pAIController = Cast<AEnemy_Controller>(a_pTreeComp.GetAIOwner());
	APawn* pEnemy = pAIController->GetPawn();

	// Get player controller anbd pawn
	APlayerController* pPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// Set blackboard key to whether or not player is in range
	pAIController->GetBlackboard()->SetValueAsBool(EnemyKeys::isPlayerInRange, pEnemy->GetDistanceTo(pPlayerController->GetPawn()) <= m_fRange);
}
