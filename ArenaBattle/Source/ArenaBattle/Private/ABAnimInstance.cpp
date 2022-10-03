// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance() 
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Book/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage"));
	
	if (ATTACK_MONTAGE.Succeeded()) {
		// For. Debug
		// ABLOG(Warning, TEXT("AttackMontage Create"));
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// TryGetOwner는 이 애니메이션을 갖고있는 폰의 객체를 가져온다.
	// 유효하지 않으면 NULL을 반환한다.
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) {
		return;
	}

	if (!IsDead) {
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		auto Character = Cast<ACharacter>(Pawn);
		if (Character) {
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}

	// Chapter 9. 충돌 설정과 데미지 전달 에서 수정
	//// Pawn이 NULL인지 유요한 포인터인지 검사.
	//if (::IsValid(Pawn)) {
	//	CurrentPawnSpeed = Pawn->GetVelocity().Size();
	//	auto Character = Cast<ACharacter>(Pawn);
	//	if (Character) {
	//		IsInAir = Character->GetMovementComponent()->IsFalling();
	//	}
	//}

	// Anim Instance의 Tick에서 Pawn에 접근해 애니메이션을 생성하는 방법이 일반적이다.

}

void UABAnimInstance::PlayAttackMontage()
{
	ABCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f);
}

void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

void UABAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UABAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	// For.Debug
	// ABLOG(Warning, TEXT("%s"), *FString::Printf(TEXT("Attack%d"), Section));
	return FName(*FString::Printf(TEXT("Attack%d"),Section));
}
