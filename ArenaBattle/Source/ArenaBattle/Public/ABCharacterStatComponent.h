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

	// Transient Ű����� �ش� �Ӽ��� ����ȭ���� ���� �����ϴ�.
	// �𸮾� ������Ʈ�� ����ȭ ����� �־� UPROPERTY �Ӽ��� �����ϰ� �ε� ����.
	// CurrentHP ���� ������ ������ �� ���� ����ǹǷ� �����ϴ� ���� �ǹ� X
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAcceess = true))
	float		CurrentHP;
};