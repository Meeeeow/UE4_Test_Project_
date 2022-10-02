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

	// Attack Task�� ���� �ִϸ��̼��� ���� ������ ����ؾ��ϴ� ���� Task �̹Ƿ�
	// ExecuteTask�� ��� ���� InProgress�� �ϴ� ��ȯ�ϰ� ������ ���� �� Task ���Ḧ �˷���� �Ѵ�.
	// �̸� �˷��ִ� �Լ��� FinishLatenTask��. 
	// Task���� �� �Լ��� ���߿� ȣ�������� ������ Behavior Tree System�� ���� Task�� ��� �ӹ��� �ȴ�.
	virtual EBTNodeResult::Type		ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void	TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool			IsAttacking = false;
};
