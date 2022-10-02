// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Attack();

	// Attack Task는 공격 애니메이션이 끝날 때까지 대기해야하는 지연 Task 이므로
	// ExecuteTask의 결과 값을 InProgress로 일단 반환하고 공격이 끝날 때 Task 종료를 알려줘야 한다.
	// 이를 알려주는 함수가 FinishLatenTask다. 
	// Task에서 이 함수를 나중에 호출해주지 않으면 Behavior Tree System은 현재 Task에 계속 머물게 된다.
	virtual EBTNodeResult::Type		ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void	TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool			IsAttacking = false;
};
