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

	// TryGetOwner�� �� �ִϸ��̼��� �����ִ� ���� ��ü�� �����´�.
	// ��ȿ���� ������ NULL�� ��ȯ�Ѵ�.
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

	// Chapter 9. �浹 ������ ������ ���� ���� ����
	//// Pawn�� NULL���� ������ ���������� �˻�.
	//if (::IsValid(Pawn)) {
	//	CurrentPawnSpeed = Pawn->GetVelocity().Size();
	//	auto Character = Cast<ACharacter>(Pawn);
	//	if (Character) {
	//		IsInAir = Character->GetMovementComponent()->IsFalling();
	//	}
	//}

	// Anim Instance�� Tick���� Pawn�� ������ �ִϸ��̼��� �����ϴ� ����� �Ϲ����̴�.

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
