// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenWorldClientGameMode.h"
#include "OpenWorldClientCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include <Blueprint/UserWidget.h>
#include <OpenWorldClient/MainControllerPC.h>
#include <OpenWorldClient/MainHUD.h>
#include "OpenWorldClient/OpenWorldGameInstance.h"
#include "OpenWorldClient/GameTcpSocketConnection.h"

AOpenWorldClientGameMode::AOpenWorldClientGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	PlayerControllerClass = AMainControllerPC::StaticClass();
	HUDClass = AMainHUD::StaticClass();
	
}

void AOpenWorldClientGameMode::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidget(StartingWidgetClass);
	//OpenWorldConnection = NewObject<AGameTcpSocketConnection>();
}

void AOpenWorldClientGameMode::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void AOpenWorldClientGameMode::SendNickname(const FString& nick)
{
	UOpenWorldGameInstance* gameInstance = GetGameInstance<UOpenWorldGameInstance>();

	gameInstance->OpenWorldConnection->SendNickname(nick);

}

void AOpenWorldClientGameMode::OnSendNicknameAck(const FString& nick)
{
	UOpenWorldGameInstance* gameInstance = GetGameInstance<UOpenWorldGameInstance>();

	gameInstance->Nickname = nick;

	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
}