// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Detect.generated.h"

// Service Node는 자신이 속한 Composite Node가 활성화 될 경우 주기적으로 TickNode 함수를 호출한다.
// 호출하는 주기는 서비스 노드 내부에 설정된 Interval 속성 값으로 지정한다.

// 모든 캐릭터를 감지하는 OverlapMultiByChannel 함수를 사용하면 된다.
// 모든 캐릭터 정보는 목록을 관리하는 데 적합한 TArray(UE의 자료구조)로 전달된다.

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
