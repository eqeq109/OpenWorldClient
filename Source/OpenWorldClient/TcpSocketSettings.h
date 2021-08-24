// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TcpSocketSettings.generated.h"

/**
 * 
 */
UCLASS(config = Engine, defaultconfig)
class OPENWORLDCLIENT_API UTcpSocketSettings : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(config, EditAnywhere, Category = "TcpSocketPlugin")
		bool bPostErrorsToMessageLog;
};
