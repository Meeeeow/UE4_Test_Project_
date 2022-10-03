// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Detect.generated.h"

// Service Node�� �ڽ��� ���� Composite Node�� Ȱ��ȭ �� ��� �ֱ������� TickNode �Լ��� ȣ���Ѵ�.
// ȣ���ϴ� �ֱ�� ���� ��� ���ο� ������ Interval �Ӽ� ������ �����Ѵ�.

// ��� ĳ���͸� �����ϴ� OverlapMultiByChannel �Լ��� ����ϸ� �ȴ�.
// ��� ĳ���� ������ ����� �����ϴ� �� ������ TArray(UE�� �ڷᱸ��)�� ���޵ȴ�.

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTService_Detect : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Detect();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
