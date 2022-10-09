// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHPChangeDelegate)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public:
	void	SetNewLevel(int32 NewLevel);
	void	SetDamage(float NewDamage);
	void	SetHP(float NewHP);
	float	GetAttack() const;
	float	GetHPRatio() const;
	int32	GetDropExp() const;

	FOnHPIsZeroDelegate	OnHPIsZero;
	FOnHPChangeDelegate OnHPChanged;
private:
	struct FABCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32		Level;

	// Transient 키워드로 해당 속성을 직렬화에서 제외 가능하다.
	// 언리얼 오브젝트는 직렬화 기능이 있어 UPROPERTY 속성을 저장하고 로딩 가능.
	// CurrentHP 값은 게임을 시작할 때 마다 변경되므로 보관하는 것은 의미 X
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAcceess = true))
	float		CurrentHP;
};
