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

using FChatMessageDelegate = TDelegate<void(const FPacketChatMessageArrived& ack)>;
using FSetNicknameAckDelegate = TDelegate<void(const FString& nick)>;

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
		void SendMove(const int16 moveState, const FVector& pos, const FRotator& rot);
	UFUNCTION()
		void SendNickname(const FString& message);
	UPROPERTY()
		int32 ConnectionIdGameServer;


	FChatMessageDelegate ChatMessageDelegate;

	FSetNicknameAckDelegate SetNicknameAckDelegate;
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
		int16 MoveState;
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


USTRUCT()
struct OPENWORLDCLIENT_API FFieldUserData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
		int UserIndex;
	UPROPERTY()
		FString UserName;
	UPROPERTY()
		FVector Position;
	UPROPERTY()
		FRotator Rotation;

};

USTRUCT()
struct OPENWORLDCLIENT_API FPacketConnectAck
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
		FString MyName;
	UPROPERTY()
		int16 ResultType;
	UPROPERTY()
		TArray<FFieldUserData> UserList;

};

USTRUCT()
struct OPENWORLDCLIENT_API FPacketChatMessageArrived
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
		int16 ChatMsgType;
	UPROPERTY()
		int32 OwnerIndex;
	UPROPERTY()
		FString Message;
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


enum class EMoveState
{
	Begin = 0,
	Idle = 1,
	MoveStart = 2,
	Jump = 3,
	End
};
