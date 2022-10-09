// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"

// ������ enum�� Blueprint���� ����ϱ� ���� UENUM(BlueprintType)�� �����Ѵ�.
UENUM(BlueprintType)
enum class ECharacterState : uint8 {
	PREINT			= 0,
	LOADING			= 1,
	READY			= 2,
	DEAD			= 3
};


DECLARE_LOG_CATEGORY_EXTERN(ArenaBattle, Log, All);

#define ABLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define ABLOG_S(Verbosity) UE_LOG(ArenaBattle, Verbosity, TEXT("%s"), *ABLOG_CALLINFO)
#define ABLOG(Verbosity, Format, ...) UE_LOG(ArenaBattle, Verbosity, TEXT("%s %s"), *ABLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define ABCHECK(Expr, ...) { if(!(Expr)) { ABLOG(Error,TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; } }