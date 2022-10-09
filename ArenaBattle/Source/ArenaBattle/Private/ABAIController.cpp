// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAIController.h"

// UE 4.20 이상부터 build.cs 에 "NavigationSystem" 모듈이 추가되었으며,
// 아래의 두 헤더를 추가해야한다.
//#include "NavigationSystem.h"
//#include "Blueprint/AIBlueprintHelperLibrary.h"

// Behavior Tree를 사용한다
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

/* Using Navigation System */
//AABAIController::AABAIController()
//{
//	RepeatInterval = 3.0f;
//}
//
//void AABAIController::OnPossess(APawn* InPawn)
//{
//	Super::OnPossess(InPawn);
//	GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AABAIController::OnRepeatTimer, RepeatInterval, true);
//}
//
//void AABAIController::OnUnPossess()
//{
//	Super::OnUnPossess();
//	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);
//}
//
//void AABAIController::OnRepeatTimer()
//{
//	auto	CurrentPawn = GetPawn();
//	ABCHECK(nullptr != CurrentPawn);
//
//	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
//
//	if (nullptr == NavSystem) {
//		return;
//	}
//
//	FNavLocation	NextLocation;
//	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.0f, NextLocation)) {
//		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
//		ABLOG(Warning, TEXT("Next Location : %s"), *NextLocation.Location.ToString());
//	}
//}

/* Using Behaviro Tree / Blackboard */

const FName AABAIController::HomePosKey(TEXT("HomePos"));
const FName AABAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AABAIController::TargetKey(TEXT("Target"));

AABAIController::AABAIController() 
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Book/AI/BB_ABCharacter.BB_ABCharacter"));
	if (BBObject.Succeeded()) {
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Book/AI/BT_ABCharacter.BT_ABCharacter"));
	if (BTObject.Succeeded()) {
		BTAsset = BTObject.Object;
	}
}

void AABAIController::OnPossess(APawn* InPawn) 
{
	Super::OnPossess(InPawn);

	// Chapter 14. 510p 에서 수정됨
	/*
	if (UseBlackboard(BBAsset, Blackboard)) {

		Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		if (!RunBehaviorTree(BTAsset)) {
			ABLOG(Error, TEXT("AI Controller couldn't run behavior tree!"));
		}
	}
	*/
}

void AABAIController::RunAI()
{
	if (UseBlackboard(BBAsset, Blackboard)) {

		Blackboard->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());
		if (!RunBehaviorTree(BTAsset)) {
			ABLOG(Error, TEXT("AI Controller couldn't run behavior tree!"));
		}
	}
}

void AABAIController::StopAI()
{
	auto	BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (nullptr != BehaviorTreeComponent) {
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}
