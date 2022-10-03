// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "ABGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FABCharacterData : public FTableRowBase 
{
	GENERATED_BODY()
	
public:
	FABCharacterData() 
		: Level(1), MaxHP(100.0f), Attack(10.0f), DropExp(10), NextExp(30) 
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32		Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float		MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float		Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32		DropExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32		NextExp;
};




/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UABGameInstance();

	virtual void	Init() override;
	
	FABCharacterData* GetABCharacterData(int32 Level);

	// 유일하게 Instance로 동작하는 ABGameInstance 클래스
	// UE는 게임 진행 중에도 비동기 방식의 에셋 로드를 지원하는 FStreamableManager 클래스를 제공해준다.
	// FStreamableManager에서 AsyncLoad를 사용해 비동기 방식으로 에셋을 로드한다.
	// FStreamableDelegate 형식의 델리게이트를 넘겨줄 경우, 에셋 로딩이 완료되면 델리게이트에 연결된 함수를 호출한다.
	FStreamableManager		StreamableManager;
private:
	UPROPERTY()
	class UDataTable*		ABCharacterTable;

	

};
