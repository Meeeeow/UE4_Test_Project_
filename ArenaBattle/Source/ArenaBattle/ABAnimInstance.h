// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

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

private:
	// 블루 프린트에서 사용하기 위해서는 BlueprintReadOnly, BlueprintReadWrite 둘 중 정할 수 있다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float	CurrentPawnSpeed;
	// UPROPERTY()를 사용한 객체는 초기화를 진행하지 않아도 된다.

	// IsFalling() 함수의 반환 값을 저장할 boolean 선언
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool	IsInAir;
};
