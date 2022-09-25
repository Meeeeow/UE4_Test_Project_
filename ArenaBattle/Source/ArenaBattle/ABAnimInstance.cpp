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

	// TryGetOwner�� �� �ִϸ��̼��� �����ִ� ���� ��ü�� �����´�.
	// ��ȿ���� ������ NULL�� ��ȯ�Ѵ�.
	auto Pawn = TryGetPawnOwner();
	
	// Pawn�� NULL���� ������ ���������� �˻�.
	if (::IsValid(Pawn)) {
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		auto Character = Cast<ACharacter>(Pawn);
		if (Character) {
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}

	// Anim Instance�� Tick���� Pawn�� ������ �ִϸ��̼��� �����ϴ� ����� �Ϲ����̴�.

}
