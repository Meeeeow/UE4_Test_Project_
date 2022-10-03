// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameInstance.h"

UABGameInstance::UABGameInstance()
{
	FString	CharacterDataPath = TEXT("/Game/Book/GameData/ABCharacterData.ABCharacterData");

	static	ConstructorHelpers::FObjectFinder<UDataTable> DT_ABCHARACTER(*CharacterDataPath);

	//ABCHECK(DT_ABCHARACTER.Succeeded());
	if (DT_ABCHARACTER.Succeeded()) {
		ABCharacterTable = DT_ABCHARACTER.Object;
		ABCHECK(ABCharacterTable->GetRowMap().Num() > 0);
	}
}

void UABGameInstance::Init()
{
	// 게임 시작 과정
	// 1. 게임 앱의 초기화 ( UGameInstance::Init )
	// 2. 레벨에 속한 액터 초기화 ( AActor::PostInitializeComponent )
	// 3. 플레이어의 로그인 ( AGameMode::PostLogin )
	// 4. 게임의 시작 ( AGameMode::StartPlay , AActor::BeginPlay )

	Super::Init();

	// ABLOG(Warning, TEXT("DropExp of Level 20 ABCharacter : %d"), GetABCharacterData(20)->DropExp);

}

FABCharacterData* UABGameInstance::GetABCharacterData(int32 Level)
{
	return ABCharacterTable->FindRow<FABCharacterData>(*FString::FromInt(Level), TEXT(""));
}
