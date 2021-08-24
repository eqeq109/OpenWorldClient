// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldGameInstance.h"

#include <OpenWorldClient/GameTcpSocketConnection.h>

void UOpenWorldGameInstance::Init()
{
	Super::Init();

	OpenWorldConnection = NewObject<AGameTcpSocketConnection>();
	OpenWorldConnection->ConnectToGameServer();
}