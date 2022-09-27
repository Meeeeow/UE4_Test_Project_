// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABAnimInstance();

	virtual	void	NativeUpdateAnimation(float DeltaSeconds) override;

	void			PlayAttackMontage();
	void			JumpToAttackMontageSection(int32 NewSection);

public:
	FOnNextAttackCheckDelegate		OnNextAttackCheck;
	FOnAttackHitCheckDelegate		OnAttackHitCheck;

	void			SetDeadAnim() { IsDead = true; }
	const bool&		GetDead() const { return IsDead; }

private:
	// Animation Notify를 감지하기 위한 함수
	// 애니메이션을 재생할 때 지정한 구간에 Notify를 호출한다.
	// UE에서는 이를 보고 자동으로 AnimInstance Class의 AnimNotify_NotifyName 멤버 함수를 호출한다.
	UFUNCTION()
	void			AnimNotify_AttackHitCheck();

	UFUNCTION()
	void			AnimNotify_NextAttackCheck();

	FName			GetAttackMontageSectionName(int32 Section);

private:
	// 블루 프린트에서 사용하기 위해서는 BlueprintReadOnly, BlueprintReadWrite 둘 중 정할 수 있다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float			CurrentPawnSpeed;
	// UPROPERTY()를 사용한 객체는 초기화를 진행하지 않아도 된다.

	// IsFalling() 함수의 반환 값을 저장할 boolean 선언
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool			IsInAir;

	// Attack Montage 변수
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage*	AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool			IsDead;
};