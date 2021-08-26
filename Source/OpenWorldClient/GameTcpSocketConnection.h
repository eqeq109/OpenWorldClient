// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TcpSocketConnection.h"
#include "GameFramework/Actor.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "Containers/Queue.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "GameTcpSocketConnection.generated.h"

using FChatMessageDelegate = TDelegate<void(const FString& message)>;

/**
 *
 */
UCLASS()
class OPENWORLDCLIENT_API AGameTcpSocketConnection : public ATcpSocketConnection
{
	GENERATED_BODY()

public:
	UFUNCTION()
		void OnConnected(int32 conneectionID);
	UFUNCTION()
		void OnDisconnected(int32 conID);
	UFUNCTION()
		void OnMessageReceived(int32 conID, TArray<uint8>& message);
	UFUNCTION(BlueprintCallable)
		void ConnectToGameServer();
	UFUNCTION()
		void SendMessage(const FString& message);
	UFUNCTION()
		void SendMove(const FVector& pos, const FRotator& rot);
	UFUNCTION()
		void SendNickname(const FString& message);
	UPROPERTY()
		int32 ConnectionIdGameServer;


	FChatMessageDelegate ChatMessageDelegate;
	//UPROPERTY()
	//FTcpSocketDisconnectDelegate DisconnectDelegate;
	//UPROPERTY()
	//FTcpSocketConnectDelegate ConnectDelegate;
	//UPROPERTY()
	//FTcpSocketReceivedMessageDelegate ReceivedDelegate;
};

USTRUCT()
struct OPENWORLDCLIENT_API FPacketChatMessageReq
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	FString Message;

};

USTRUCT()
struct OPENWORLDCLIENT_API FPacketChatMessageAck
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FString Message;

};

USTRUCT()
struct OPENWORLDCLIENT_API FPacketPlayerMoveReq
{
	GENERATED_USTRUCT_BODY()
public:
	//FVector
	UPROPERTY()
	float X;
	UPROPERTY()
	float Y;
	UPROPERTY()
	float Z;
	//FRotator
	UPROPERTY()
	float Pitch;
	UPROPERTY()
	float Yaw;
	UPROPERTY()
	float Roll;
};

USTRUCT()
struct OPENWORLDCLIENT_API FPacketCharacterMove
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	int Index;
	//FVector
	UPROPERTY()
	float X;
	UPROPERTY()
	float Y;
	UPROPERTY()
	float Z;
	//FRotator
	UPROPERTY()
	float Pitch;
	UPROPERTY()
	float Yaw;
	UPROPERTY()
	float Roll;
};

USTRUCT()
struct OPENWORLDCLIENT_API FPacketMyConnectAck
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	FString UserName;
	UPROPERTY()
	FVector StartPos;
	UPROPERTY()
	FRotator Rotation;
};
USTRUCT()
struct OPENWORLDCLIENT_API FPacketSetNicknameAck
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FString UserName;
	UPROPERTY()
		int16 ResultType;
};

USTRUCT()
struct OPENWORLDCLIENT_API FPacketSetNicknameReq
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FString UserName;
};

USTRUCT()
struct OPENWORLDCLIENT_API FPacketNewClientAck
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	int Index;

	UPROPERTY()
		FString UserName;
};

enum class EProtocolType
{
	Begin = 0,
	ChatMsgReq = 1,
	ChatMsgAck,
	ConnectReq,
	ConnectAck,
	NewClient,
	PlayerMoveReq,
	PlayerMoveAck,
	CharacterMove,
	SetNicknameReq,
	SetNicknameAck,
	End
};
