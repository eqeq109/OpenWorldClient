// Fill out your copyright notice in the Description page of Project Settings.


#include "GameTcpSocketConnection.h"
#include "Kismet/GameplayStatics.h"
#include "OpenWorldClient/MainControllerPC.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "IPAddress.h"
#include "Sockets.h"
#include "HAL/RunnableThread.h"
#include "Async/Async.h"
#include <string>
#include "Logging/MessageLog.h"
#include "HAL/UnrealMemory.h"
#include "TcpSocketSettings.h"
#include <Runtime/Core/Public/Serialization/BufferArchive.h>
#include "OpenWorldClient/OpenWorldClientGameMode.h"
#include <Runtime/JsonUtilities/Public/JsonObjectConverter.h>

void AGameTcpSocketConnection::ConnectToGameServer()
{
	ConnectionIdGameServer = 0;
	if (IsConnected(ConnectionIdGameServer))
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Can't connect Second time. We're already connected!"));
		return;
	}

	
	DisconnectedDelegate.BindUObject(this, &AGameTcpSocketConnection::OnDisconnected);

	ConnectedDelegate.BindUObject(this, &AGameTcpSocketConnection::OnConnected);
	MessageReceivedDelegate.BindUObject(this, &AGameTcpSocketConnection::OnMessageReceived);

	Connect("222.107.110.135", 9000, ConnectionIdGameServer);
}

void AGameTcpSocketConnection::OnConnected(int32 conId)
{
	UE_LOG(LogTemp, Log, TEXT("Log: Connected to server"));
}

void AGameTcpSocketConnection::OnDisconnected(int32 conId)
{
	UE_LOG(LogTemp, Log, TEXT("Log: Disconnected"));
}

/// <summary>
/// 서버에서 받은 메시지를 처리하는 부분
/// </summary>
/// <param name="ConId"></param>
/// <param name="Message"></param>
void AGameTcpSocketConnection::OnMessageReceived(int32 conId, TArray<uint8>& message)
{
	UE_LOG(LogTemp, Log, TEXT("Log: Received message"));

	while (message.Num() != 0)
	{
		int32 header = PopInt(message);
		if (header == -1)
			return;
		TArray<uint8> body;
		if (!PopBytes(header, message, body))
		{
			continue;
		}
		EProtocolType type;
		type = (EProtocolType)PopInt16(body);
		FString stringData;
		switch (type)
		{
		case EProtocolType::ChatMsgAck:
		{
			int16 msgLength = PopInt16(body);

			stringData = PopString(body, msgLength);

			ChatMessageDelegate.ExecuteIfBound(stringData);
		}
			break;
		case EProtocolType::SetNicknameAck:
		{
			//AOpenWorldClientGameMode* gameMode = (AOpenWorldClientGameMode*)GetWorld()->GetAuthGameMode();
			int16 msgLength = PopInt16(body);

			stringData = PopString(body, msgLength);
			FPacketSetNicknameAck ack;
			//DeserializeJsonToStruct(&ack, stringData);
			FJsonObjectConverter::JsonObjectStringToUStruct(stringData, &ack, 0, 0);
			//gameMode->OnSendNicknameAck(ack.UserName);
			//FBufferArchive archive();
			//ack.StaticStruct().serial
			//DeserializeObject((UObject)ack);
			//ack.StaticStruct()
			UE_LOG(LogTemp, Log, TEXT("Log: Received message"));
		}
			break;
		}   
	}
}
/// <summary>
/// 채팅 메시지 보내기
/// 패킷 형식에 맞춰서 버퍼 세팅
/// </summary>
/// <param name="message"></param>
void AGameTcpSocketConnection::SendMessage(const FString& message)
{
	UE_LOG(LogTemp, Log, TEXT("Log: SendMessage"));

	TArray<uint8> buffer;
	TArray<uint8> temp;
	//프로토콜 타입
	int16 type = (int16)EProtocolType::ChatMsgReq;
	TArray<uint8> typeToBytes = ConvInt16ToBytes(type);
	temp.Append(typeToBytes);
	//문자열 길이
	int16 msgLength = (int16)message.Len();
	TArray<uint8> msgLengthToBytes = ConvInt16ToBytes(msgLength);
	temp.Append(msgLengthToBytes);
	//문자열 
	TArray<uint8> msgToBytes = ConvStringToBytes(message);
	temp.Append(msgToBytes);
	//전체 버퍼 길이
	int32 header = temp.Num() + 4;
	TArray<uint8> headerToBytes = ConvIntToBytes(header);
	buffer.Append(headerToBytes);
	buffer.Append(temp);
	SendData(0, buffer);
}

void AGameTcpSocketConnection::SendMove(const FVector& pos, const FRotator& rot)
{
	UE_LOG(LogTemp, Log, TEXT("Log: SendMove"));

	FPacketPlayerMoveReq packet;
	packet.X = pos.X;
	packet.Y = pos.Y;
	packet.Z = pos.Z;

	packet.Pitch = rot.Pitch;
	packet.Yaw = rot.Yaw;
	packet.Roll = rot.Roll;
	FBufferArchive archive(true);
	FPacketPlayerMoveReq::StaticStruct()->SerializeBin(archive, &packet);

	TArray<uint8> data = archive;
	TArray<uint8> buffer;
	TArray<uint8> temp;
	//프로토콜 타입
	int16 type = (int16)EProtocolType::PlayerMoveAck;
	TArray<uint8> typeToBytes = ConvInt16ToBytes(type);
	temp.Append(typeToBytes);

	temp.Append(data);
	//전체 버퍼 길이
	int32 header = temp.Num() + 4;
	TArray<uint8> headerToBytes = ConvIntToBytes(header);
	buffer.Append(headerToBytes);
	buffer.Append(temp);
	SendData(0, buffer);
}

void AGameTcpSocketConnection::SendNickname(const FString& message)
{
	UE_LOG(LogTemp, Log, TEXT("Log: SendNickname"));

	TArray<uint8> buffer;
	TArray<uint8> temp;

	//프로토콜 타입
	int16 type = (int16)EProtocolType::SetNicknameReq;
	TArray<uint8> typeToBytes = ConvInt16ToBytes(type);
	temp.Append(typeToBytes);

	FPacketSetNicknameReq packet;

	packet.UserName = message;//TCHAR_TO_UTF8(*message);
	
	//기존 구조체 직렬화
	/*FBufferArchive archive(true);
	FPacketSetNicknameReq::StaticStruct()->SerializeBin(archive, &packet);*/

	//TArray<uint8> data = archive;

	//구조체 json 직렬화(성능상 단점이 있지만 간편)
	FString json;
	FJsonObjectConverter::UStructToJsonObjectString(packet, json, 0, 0);


	//문자열 길이
	int16 msgLength = (int16)json.Len();
	TArray<uint8> msgLengthToBytes = ConvInt16ToBytes(msgLength);
	temp.Append(msgLengthToBytes);

	//json 문자열
	TArray<uint8> jsonToBytes = ConvStringToBytes(json);
	temp.Append(jsonToBytes);

	//전체 버퍼 길이
	int32 header = temp.Num() + 4;
	TArray<uint8> headerToBytes = ConvIntToBytes(header);
	buffer.Append(headerToBytes);
	buffer.Append(temp);
	SendData(0, buffer);
}

