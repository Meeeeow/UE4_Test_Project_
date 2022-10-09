// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"
#include "ABCharacterStatComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "ABCharacterWidget.h"
#include "ABAIController.h"
#include "ABCharacterSetting.h"
#include "ABGameInstance.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABHUDWidget.h"

// Sets default values
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));


	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());

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

	// ĸ�� ������Ʈ�� ABCharacter Collision Preset�� ����ϵ��� ����
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));

	// ���� ������ ���� ���� �ʱ�ȭ
	AttackRange = 200.0f;
	AttackRadius = 50.0f;

	// Chapter 10. ������ ���ڿ� ���� ����
	// �����ڰ� �ƴ� Weapon Actor�� ���� �� �����ϴ� ���·� �ٲ�
	//// ���� ����
	//// ���⸦ ������ ���̷�Ż hand_rSocket�� �̸� ����
	//FName WeaponSocket(TEXT("hand_rSocket"));

	//if (GetMesh()->DoesSocketExist(WeaponSocket)) {
	//	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
	//	// ���� ���
	//	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_BlackKnight/SK_Blade_BlackKnight.SK_Blade_BlackKnight"));

	//	if (SK_WEAPON.Succeeded()) {
	//		// SK_WEAPON ���� �����ϸ� Weapon�� �ش� ������Ʈ�� ���
	//		Weapon->SetSkeletalMesh(SK_WEAPON.Object);
	//	}
	//	// ����� ���⸦ hand_rSocket�� ����
	//	Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	//}

	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));

	if (UI_HUD.Succeeded()) {
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}

	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	auto	DefaultSetting = GetDefault<UABCharacterSetting>();

	if (DefaultSetting->CharacterAssets.Num() > 0) {

		for (auto CharacterAsset : DefaultSetting->CharacterAssets) {
			ABLOG(Warning, TEXT("Character Asset : %s"), *CharacterAsset.ToString());
		}
	}

	/*
	DeadTime = 0.0f; 
	*/

	AssetIndex = 4;

	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	SetCanBeDamaged(false);

	DeadTimer = 5.0f;

}

void AABCharacter::SetCharacterState(ECharacterState NewState)
{
	ABCHECK(NewState != CurrentState);
	CurrentState = NewState;

	switch (CurrentState)
	{
	case ECharacterState::LOADING:
	{
		if (bIsPlayer) {
			DisableInput(ABPlayerController);

			ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);

			auto	ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
			ABCHECK(ABPlayerState != nullptr);

			CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());
		}
		SetActorHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		SetCanBeDamaged(false);
		break;
	}
	case ECharacterState::READY:
	{
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		SetCanBeDamaged(true);

		CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
			SetCharacterState(ECharacterState::DEAD);
			});
		auto	CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(nullptr != CharacterWidget);

		CharacterWidget->BindCharacterStat(CharacterStat);

		if (bIsPlayer) {
			SetControlMode(EControlMode::DIABLO);
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			EnableInput(ABPlayerController);
		}
		else {
			SetControlMode(EControlMode::NPC);
			GetCharacterMovement()->MaxWalkSpeed = 400.0f;
			ABAIController->RunAI();
		}
		break;
	}
	case ECharacterState::DEAD:
	{
		SetActorEnableCollision(false);
		GetMesh()->SetHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		ABAnim->SetDeadAnim();
		SetCanBeDamaged(false);

		if (bIsPlayer) {
			DisableInput(ABPlayerController);
		}
		else {
			ABAIController->StopAI();
		}

		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void {

			if (bIsPlayer) {
				ABPlayerController->RestartLevel();
			}
			else {
				Destroy();
			}

			}), DeadTimer, false);

		break;
	}
	default:
		break;
	}
}

ECharacterState AABCharacter::GetCharacterState() const
{
	return CurrentState;
}

int32 AABCharacter::GetExp() const
{
	return CharacterStat->GetDropExp();
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//FName WeaponSocket(TEXT("hand_rSocket"));
	//Weapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);

	//if (nullptr != Weapon) {
	//	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	//}

	
	

	// Chapter 14. 506p ���� ������.
	/*
	auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
	
	if (nullptr != CharacterWidget) {
		CharacterWidget->BindCharacterStat(CharacterStat);
	}

	if (!IsPlayerControlled()) {
		auto	DefaultSetting = GetDefault<UABCharacterSetting>();
		int32	RandIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);

		CharacterAssetToLoad = DefaultSetting->CharacterAssets[RandIndex];

		auto	ABGameInstance = Cast<UABGameInstance>(GetGameInstance());

		if (nullptr != ABGameInstance) {
			AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(
				CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadComplete));
		}
	}*/

	bIsPlayer = IsPlayerControlled();
	if (bIsPlayer) {
		ABPlayerController = Cast<AABPlayerController>(GetController());
		ABCHECK(nullptr != ABPlayerController);
	}
	else {
		ABAIController = Cast<AABAIController>(GetController());
		ABCHECK(nullptr != ABAIController);
	}

	auto	DefaultSetting = GetDefault<UABCharacterSetting>();

	if (bIsPlayer) {
		AssetIndex = 4;
	}
	else {
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	}

	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	auto	ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);

	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(
		CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadComplete));

	SetCharacterState(ECharacterState::LOADING);
}

void AABCharacter::SetControlMode(EControlMode NewControlMode)
{
	CurrentControlMode = NewControlMode;

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		/* InterpTo �Լ� ����� ���� ArmLengthTo�� ��ü
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
		// bOrientRotationMovement�� ĳ���Ͱ� �����̴� �������� ĳ���͸� �ڵ����� ȸ�� �����ִ� ����̴�.
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// ĳ���Ͱ� ȸ���ϴ� �ӵ��� ������ �� �� �ִ�.
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
		// ĳ���Ͱ� ��Ʈ�� ȸ���� ����Ű�� �������� �ε巴�� ȸ���ϴ� ����̴�.
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		break;
	case EControlMode::DIABLO:
		/* InterpTo �Լ� ����� ���� ArmLengthTo�� ArmRotationTo�� ��ü
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
	case EControlMode::NPC:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 140.0f, 0.0f);
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
		/* 4.24 ���� ���� ����.
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


	//if (ABAnim->GetDead()) {
	//	DeadTime += DeltaTime;
	//	if (DeadTime > 2.0f) {
	//		//Weapon->IsDead();
	//		if (CurrentWeapon != nullptr) {
	//			CurrentWeapon->IsDead();
	//		}
	//		Destroy();
	//	}
	//}

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

	// Chapter 14. 508p���� ������
	/*CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
		ABLOG(Warning, TEXT("OnHPIsZero"));
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
		});*/


	// 4.21 �������� ������ �ʱ�ȭ ������ PostInitializeComponents ���� BeginPlay�� ���� ��
	/*
	auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());

	if (nullptr != CharacterWidget) {
		CharacterWidget->BindCharacterStat(CharacterStat);
	}
	*/

}

float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	CharacterStat->SetDamage(FinalDamage);

	if (CurrentState == ECharacterState::DEAD) {

		if (EventInstigator->IsPlayerController()) {

			auto ABController = Cast<AABPlayerController>(EventInstigator);
			ABCHECK(nullptr != ABController, 0.0f);
			ABController->NPCKill(this);

		}

	}
	// Chapter 11. 370p���� ����
	/*if (FinalDamage > 0.0f) {
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	}*/
	return FinalDamage;
}

void AABCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Chapter 14. 508p ���� ������
	/*if (IsPlayerControlled()) {
		SetControlMode(EControlMode::DIABLO);
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else {
		SetControlMode(EControlMode::NPC);
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}*/
}

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponent�� ���� ��, �׼ǵ��� �������� �� �ִ�.

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AABCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AABCharacter::Attack);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABCharacter::Turn);
}

bool AABCharacter::CanSetWeapon()
{
	return (nullptr == CurrentWeapon);
}

void AABCharacter::SetWeapon(AABWeapon* NewWeapon)
{
	ABCHECK(nullptr != NewWeapon && nullptr == CurrentWeapon);
	FName WeaponSocket(TEXT("hand_rSocket"));
	if (nullptr != NewWeapon) {
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
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
	if (nullptr == CurrentWeapon && (CurrentControlMode != EControlMode::NPC) ) {
		return;
	}


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

	OnAttackEnd.Broadcast();
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
	// ������ �浹�� Ž���� ��� ���õ� ������ ���� ����ü
	FHitResult	HitResult;
	// Structure that defines parameters passed into collision function
	// �浹 �Լ��� ���޵� �Ű������� �����ϴ� ����ü
	FCollisionQueryParams Params(NAME_None, false, this);
	
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult /*FHitResult& OutHit */,
		GetActorLocation() /* Start */,
		GetActorLocation() + GetActorLocation() * AttackRange /* End */,
		FQuat::Identity /* Rotation */,
		ECollisionChannel::ECC_GameTraceChannel2 /* DefaultEngine.ini ���� Ȯ�� ����, ECC 2���� Attack */,
		FCollisionShape::MakeSphere(AttackRadius) /* �浹ü Shape*/,
		Params);

	// Debug Draw
#if ENABLE_DRAW_DEBUG

	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float	HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat	CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor	DrawColor = (bResult) ? FColor::Green : FColor::Red;
	float	DebugLifeTime = 5.0f;

	// DrawCapsule�� ����� ���� ������ ������ ǥ���Ѵ�.
	// ĸ���� ������(AttackRadius)�� Ž�� ���� ��ġ���� �� ��ġ�� ���ϴ� ���͸� ���Ѵ�.
	// ������ ���� ��ġ�� ���� ���� ������ �����ϸ� ���ϴ� ũ���� ĸ�� ����� ���� �� �ִ�.
	// ĸ���� ĳ���� �ü� �������� ���´ٴ� �� ĸ���� ��� ���� ���Ͱ� �ü� ����� ��ġ �Ѵٴ� ���̴�. 
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
		// �浹 ������ ���Ͱ� ��ȿ�ϸ�
		if (HitResult.Actor.IsValid()) {
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(CharacterStat->GetAttack(), DamageEvent, GetController(), this);
		}
	}
}

void AABCharacter::OnAssetLoadComplete()
{
	USkeletalMesh*		AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());

	AssetStreamingHandle.Reset();

	ABCHECK(nullptr != AssetLoaded);
	GetMesh()->SetSkeletalMesh(AssetLoaded);

	SetCharacterState(ECharacterState::READY);

	// Chapter 14. 508p���� ������
	/*if (nullptr != AssetLoaded) {
		GetMesh()->SetSkeletalMesh(AssetLoaded);
	}*/
}