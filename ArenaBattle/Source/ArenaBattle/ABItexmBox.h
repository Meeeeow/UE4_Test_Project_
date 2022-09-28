// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABItexmBox.generated.h"

UCLASS()
class ARENABATTLE_API AABItexmBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABItexmBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	UPROPERTY(VisibleAnywhere, Category = Box)
	UBoxComponent*					Trigger;

	UPROPERTY(VisibleAnywhere, Category = Box)
	UStaticMeshComponent*			Box;

	// Particle Component���� �����ϴ� OnSystemFinished Delegate�� �����Ѵ�.
	// OnsystemFinished Delegate�� Dyanmic �����̹Ƿ� Lambda�� ���ε��� �Ұ��� UFUNCION()���� ���ε� �ؾ��Ѵ�.
	UPROPERTY(VisibleAnywhere, Category = Effect)
	UParticleSystemComponent*		Effect;

	UPROPERTY(EditInstanceOnly, Category = Box)
	TSubclassOf<class AABWeapon>	WeaponItemClass;

private:
	UFUNCTION()
	void	OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor
			, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void	OnEffectFinished(class UParticleSystemComponent* PSystem);

};
