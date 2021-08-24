// Fill out your copyright notice in the Description page of Project Settings.


#include "MainControllerPC.h"
#include "Kismet/GameplayStatics.h"
#include "MainHUD.h"
#include "OpenWorldClientGameMode.h"
#include <OpenWorldClient/GameTcpSocketConnection.h>
#include <OpenWorldClient/OpenWorldGameInstance.h>

void AMainControllerPC::BeginPlay()
{
	Super::BeginPlay();

	UOpenWorldGameInstance* gameInstance = GetGameInstance<UOpenWorldGameInstance>();
	
	gameInstance->OpenWorldConnection->ChatMessageDelegate.BindUObject(this, &AMainControllerPC::ReceiveMessage);

	SetShowMouseCursor(false);
	SetInputMode(FInputModeGameOnly());

	//SampleDelegate.bind
}

void AMainControllerPC::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("Chat"), EInputEvent::IE_Pressed, this, &AMainControllerPC::FocusChatInputText);
}

void AMainControllerPC::SendMessage(const FText& text)
{
	FString userName = "BoGyeom";
	FString message = FString::Printf(TEXT("%s : %s"), *userName, *text.ToString());

	SendServerMessage(message);
}

void AMainControllerPC::FocusChatInputText()
{
	AMainHUD* hud = GetHUD<AMainHUD>();
	if (hud == nullptr) return;

	FInputModeUIOnly inputMode;
	inputMode.SetWidgetToFocus(hud->GetChatInputTextObject());

	SetInputMode(inputMode);
}

void AMainControllerPC::FocusGame()
{
	SetInputMode(FInputModeGameOnly());
}

void AMainControllerPC::SendServerMessage(const FString& message)
{
	UOpenWorldGameInstance* gameInstance = GetGameInstance<UOpenWorldGameInstance>();
	gameInstance->OpenWorldConnection->SendMessage(message);
}

void AMainControllerPC::ReceiveMessage(const FString& message)
{
	AMainHUD* hud = GetHUD<AMainHUD>();
	if (hud == nullptr) return;

	hud->AddChatMessage(message);
}