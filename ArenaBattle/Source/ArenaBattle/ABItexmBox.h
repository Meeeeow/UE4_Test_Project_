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

	// Particle Component에서 제공하는 OnSystemFinished Delegate에 연결한다.
	// OnsystemFinished Delegate는 Dyanmic 형식이므로 Lambda로 바인딩이 불가해 UFUNCION()으로 바인딩 해야한다.
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
