// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Character.h"
#include "ABCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class ARENABATTLE_API AABCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	enum class EControlMode : unsigned int {
		GTA			= 0,
		DIABLO		= 1,
		NPC			= 2
	};

	void			SetControlMode(EControlMode NewControlMode);

	EControlMode	CurrentControlMode = EControlMode::GTA;
	FVector			DirectionToMove = FVector::ZeroVector;

	// FMath에서 제공하는 InterpTo 함수를 사용하기 위한 변수 ArmRotationTo. 
	// float 형을 처리할 땐 FInterpTo, Vector는 VInterpTo, Rotator는 RInterpTo
	// SpringArm의 길이와 회전 값이 목표 값까지 각각의 목표 설정 값으로 변하는 함수다.
	float			ArmLengthTo = 0.0f;
	FRotator		ArmRotationTo = FRotator::ZeroRotator;
	float			ArmLengthSpeed = 0.0f;
	float			ArmRotationSpeed = 0.0f;

public:	
	// Called every frame
	virtual void	Tick(float DeltaTime) override;
	virtual	void	PostInitializeComponents() override;

	virtual	float	TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void	PossessedBy(AController* NewController) override;

	// Called to bind functionality to input
	virtual void	SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool			CanSetWeapon();
	void			SetWeapon(class AABWeapon* NewWeapon);

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class AABWeapon*	CurrentWeapon;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UABCharacterStatComponent*		CharacterStat;

	//UPROPERTY(VisibleAnywhere, Category = Weapon)
	//USkeletalMeshComponent*		Weapon;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent*		SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent*			Camera;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent*		HPBarWidget;

private:
	void	UpDown(float NewAxisValue);
	void	LeftRight(float NewAxisValue);
	void	LookUp(float NewAxisValue);
	void	Turn(float NewAxisValue);

	void	ViewChange();

public:	// AIController에서 공격 명령을 내릴 수 있도록 Public으로 접근자 변경
	void	Attack();
	FOnAttackEndDelegate	OnAttackEnd;
private:

	UFUNCTION()
	void	OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void	AttackStartComboState();
	void	AttackEndComboState();
	void	AttackCheck();

	void	OnAssetLoadComplete();

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool	IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool	CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool	IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32	CurrentCombo;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32	MaxCombo;

	UPROPERTY()
	class   UABAnimInstance*	ABAnim;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float	AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float	AttackRadius;
	
	// For. Test
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Dead, Meta = (AllowPrivateAccess = true))
	float	DeadTime;

	FSoftObjectPath							CharacterAssetToLoad = FSoftObjectPath(nullptr);
	TSharedPtr<struct FStreamableHandle>	AssetStreamingHandle;

};
