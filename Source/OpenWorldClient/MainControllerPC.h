// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OpenWorldClient/GameTcpSocketConnection.h"
#include "MainControllerPC.generated.h"


/**
 * 
 */
UCLASS()
class OPENWORLDCLIENT_API AMainControllerPC : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	void SendMessage(const FText& text);

	UFUNCTION()
		void FocusChatInputText();

	UFUNCTION()
		void FocusGame();
	//UPROPERTY()
		//class AGameTcpSocketConnection* OpenWorldConnection;
	UFUNCTION()
		void ReceiveMessage(const FPacketChatMessageArrived& ack);


private:
	UFUNCTION()
		void SendServerMessage(const FString& message);

};
