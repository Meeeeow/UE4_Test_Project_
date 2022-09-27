// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "DrawDebugHelpers.h"

// Sets default values
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -80.f), FRotator(0.0f, -90.f, 0.0f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));

	if (SK_CARDBOARD.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> IB_WARRIOR_ANIM(TEXT("/Game/Book/Animations/IB_WarriorAnimBluePrint.IB_WarriorAnimBluePrint_C"));
	if (IB_WARRIOR_ANIM.Succeeded()) {
		GetMesh()->SetAnimClass(IB_WARRIOR_ANIM.Class);
	}

	SetControlMode(EControlMode::DIABLO);

	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	IsAttacking = false;

	MaxCombo = 4;
	AttackEndComboState();

	// 캡슐 컴포넌트가 ABCharacter Collision Preset을 사용하도록 설정
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));

	// 공격 범위를 위한 변수 초기화
	AttackRange = 200.0f;
	AttackRadius = 50.0f;
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AABCharacter::SetControlMode(EControlMode NewControlMode)
{
	CurrentControlMode = NewControlMode;

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		/* InterpTo 함수 사용을 위해 ArmLengthTo로 대체
		SpringArm->TargetArmLength = 450.0f;
		SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		*/
		ArmLengthTo = 450.0f;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;
		// bOrientRotationMovement는 캐릭터가 움직이는 방향으로 캐릭터를 자동으로 회전 시켜주는 기능이다.
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// 캐릭터가 회전하는 속도를 지정해 줄 수 있다.
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
		// 캐릭터가 컨트롤 회전이 가리키는 방향으로 부드럽게 회전하는 기능이다.
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		break;
	case EControlMode::DIABLO:
		/* InterpTo 함수 사용을 위해 ArmLengthTo와 ArmRotationTo로 대체
		SpringArm->TargetArmLength = 800.0f;
		SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
		*/
		ArmLengthTo = 800.0f;
		ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		break;
	default:
		break;
	}

}

// Called every frame
void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		break;
	case AABCharacter::EControlMode::DIABLO:
		/* 4.24 이후 변경 사항.
		Deprecated. RelativeRotation;
		SpringArm->RelativeRotation = FMath::RInterpTo(SpringArm->ReleativeRotation, ArmRotationTo, DeltaTime, ArmRotationSpeed);
		*/
		
		SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed));

		break;
	}

	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		break;
	case AABCharacter::EControlMode::DIABLO:
		if (DirectionToMove.SizeSquared() > 0.0f)
		{
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
		break;
	}

	if (ABAnim->GetDead()) {
		DeadTime += DeltaTime;
		if (DeadTime > 2.0f) {
			Destroy();
			EndPlay(EEndPlayReason::Type::RemovedFromWorld);
		}
	}

}

void AABCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ABAnim);

	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);

	ABAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn) {
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);
}

float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	if (FinalDamage > 0.0f) {
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	}

	return FinalDamage;
}

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponent를 통해 축, 액션등을 정의해줄 수 있다.

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AABCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AABCharacter::Attack);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABCharacter::Turn);
}

void AABCharacter::UpDown(float NewAxisValue)
{
	if (IsAttacking) {
		NewAxisValue /= 2.0f;
	}

	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		DirectionToMove.X = NewAxisValue;
		break;
	default:
		break;
	}
}

void AABCharacter::LeftRight(float NewAxisValue)
{
	if (IsAttacking) {
		NewAxisValue /= 2.0f;
	}

	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		DirectionToMove.Y = NewAxisValue;
		break;
	default:
		break;
	}
}

void AABCharacter::LookUp(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddControllerPitchInput(NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		break;
	default:
		break;
	}
}

void AABCharacter::Turn(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddControllerYawInput(NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		break;
	default:
		break;
	}
}

void AABCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EControlMode::DIABLO);
		break;
	case AABCharacter::EControlMode::DIABLO:
		GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
		SetControlMode(EControlMode::GTA);
		break;
	default:
		break;
	}

}

void AABCharacter::Attack()
{
	if (IsAttacking) {
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo) {
			IsComboInputOn = true;
		}
	}
	else {
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
}

void AABCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
	// For. Debug
	// ABLOG(Warning, TEXT("ABCharacter; CurrentCombo %d"), CurrentCombo);
}

void AABCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AABCharacter::AttackCheck()
{
	// 물리적 충돌이 탐지된 경우 관련된 정보를 담을 구조체
	FHitResult	HitResult;
	// Structure that defines parameters passed into collision function
	// 충돌 함수로 전달된 매개변수를 정의하는 구조체
	FCollisionQueryParams Params(NAME_None, false, this);
	
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult /*FHitResult& OutHit */,
		GetActorLocation() /* Start */,
		GetActorLocation() + GetActorLocation() * AttackRange /* End */,
		FQuat::Identity /* Rotation */,
		ECollisionChannel::ECC_GameTraceChannel2 /* DefaultEngine.ini 에서 확인 가능, ECC 2번이 Attack */,
		FCollisionShape::MakeSphere(AttackRadius) /* 충돌체 Shape*/,
		Params);

	// Debug Draw
#if ENABLE_DRAW_DEBUG

	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float	HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat	CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor	DrawColor = (bResult) ? FColor::Green : FColor::Red;
	float	DebugLifeTime = 5.0f;

	// DrawCapsule을 사용해 원이 움직인 궤적을 표현한다.
	// 캡슐의 반지름(AttackRadius)을 탐색 시작 위치에서 끝 위치로 향하는 벡터를 구한다.
	// 벡터의 중점 위치와 벡터 길이 절반을 대입하면 원하는 크기의 캡슐 모양을 구할 수 있다.
	// 캡슐이 캐릭터 시선 방향으로 눕는다는 건 캡슐의 상단 방향 벡터가 시선 방향과 일치 한다는 것이다. 
	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

#endif
	if (bResult) {
		// 충돌 감지된 액터가 유효하면
		if (HitResult.Actor.IsValid()) {
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(50.0f, DamageEvent, GetController(), this);
		}
	}
}

