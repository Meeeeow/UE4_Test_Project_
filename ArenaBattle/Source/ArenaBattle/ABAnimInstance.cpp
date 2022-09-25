// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance() 
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// TryGetOwner는 이 애니메이션을 갖고있는 폰의 객체를 가져온다.
	// 유효하지 않으면 NULL을 반환한다.
	auto Pawn = TryGetPawnOwner();
	
	// Pawn이 NULL인지 유요한 포인터인지 검사.
	if (::IsValid(Pawn)) {
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		auto Character = Cast<ACharacter>(Pawn);
		if (Character) {
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}

	// Anim Instance의 Tick에서 Pawn에 접근해 애니메이션을 생성하는 방법이 일반적이다.

}
