// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTDecorator_IsInAttackRange : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_IsInAttackRange();

protected:
	// Decorator Class는 CalculateRawConditionValue 함수를 상속받아 조건이 충족되는지 파악할 수 있다.
	// 이 함수는 const로 Decorator Class의 멤버 변수 값은 변경할 수 없다.

	virtual bool	CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
