// Fill out your copyright notice in the Description page of Project Settings.


#include "MainControllerPC.h"
#include "Kismet/GameplayStatics.h"
#include "MainHUD.h"
#include "OpenWorldClientGameMode.h"
#include "OpenWorldClient/GameTcpSocketConnection.h"
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

void AMainControllerPC::ReceiveMessage(const FPacketChatMessageArrived& ack)
{
	AMainHUD* hud = GetHUD<AMainHUD>();
	if (hud == nullptr) return;

	//TODO: 접속시에 받은 필드데이터 기반으로 닉네임과 메시지를 조합하여 출력
	FString fullMessage = ack.Message;

	hud->AddChatMessage(fullMessage);
}