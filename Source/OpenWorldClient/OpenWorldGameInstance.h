// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OpenWorldGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDCLIENT_API UOpenWorldGameInstance : public UGameInstance
{
	GENERATED_BODY()

		virtual void Init() override;
public:
	UPROPERTY()
		class AGameTcpSocketConnection* OpenWorldConnection;

public:
	UPROPERTY()
		FString Nickname;

};
