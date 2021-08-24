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

void AGameTcpSocketConnection::ConnectToGameServer()
{
	ConnectionIdGameServer = 0;
	if (isConnected(ConnectionIdGameServer))
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Can't connect Second time. We're already connected!"));
		return;
	}

	
	DisconnectedDelegate.BindUObject(this, &AGameTcpSocketConnection::OnDisconnected);

	ConnectedDelegate.BindUObject(this, &AGameTcpSocketConnection::OnConnected);
	MessageReceivedDelegate.BindUObject(this, &AGameTcpSocketConnection::OnMessageReceived);

	Connect("222.107.110.135", 9000, ConnectionIdGameServer);
}

void AGameTcpSocketConnection::OnConnected(int32 ConId)
{
	UE_LOG(LogTemp, Log, TEXT("Log: Connected to server"));
}

void AGameTcpSocketConnection::OnDisconnected(int32 ConId)
{
	UE_LOG(LogTemp, Log, TEXT("Log: Disconnected"));
}

/// <summary>
/// �������� ���� �޽����� ó���ϴ� �κ�
/// </summary>
/// <param name="ConId"></param>
/// <param name="Message"></param>
void AGameTcpSocketConnection::OnMessageReceived(int32 ConId, TArray<uint8>& Message)
{
	UE_LOG(LogTemp, Log, TEXT("Log: Received message"));

	while (Message.Num() != 0)
	{
		int32 header = Message_ReadInt(Message);
		if (header == -1)
			return;
		TArray<uint8> body;
		if (!Message_ReadBytes(header, Message, body))
		{
			continue;
		}
		EProtocolType type;
		type = (EProtocolType)Message_ReadInt16(body);
		switch (type)
		{
		case EProtocolType::ChatMsgAck:
			int16 msgLength = Message_ReadInt16(body);

			FString message = Message_ReadString(body, msgLength);
			
			ChatMessageDelegate.ExecuteIfBound(message);
			break;
		//case EProtocolType::SetNicknameAck:
		//	/*AOpenWorldClientGameMode* gameMode = (AOpenWorldClientGameMode*)GetWorld()->GetAuthGameMode();
		//	gameMode->OnSendNicknameAck()*/
		//	break;
		}   
	}
}
/// <summary>
/// ä�� �޽��� ������
/// ��Ŷ ���Ŀ� ���缭 ���� ����
/// </summary>
/// <param name="message"></param>
void AGameTcpSocketConnection::SendMessage(const FString& message)
{
	UE_LOG(LogTemp, Log, TEXT("Log: SendMessage"));

	TArray<uint8> buffer;
	TArray<uint8> temp;
	//�������� Ÿ��
	int16 type = (int16)EProtocolType::ChatMsgReq;
	TArray<uint8> typeToBytes = Conv_Int16ToBytes(type);
	temp.Append(typeToBytes);
	//���ڿ� ����
	int16 msgLength = (int16)message.Len();
	TArray<uint8> msgLengthToBytes = Conv_Int16ToBytes(msgLength);
	temp.Append(msgLengthToBytes);
	//���ڿ� 
	TArray<uint8> msgToBytes = Conv_StringToBytes(message);
	temp.Append(msgToBytes);
	//��ü ���� ����
	int32 header = temp.Num() + 4;
	TArray<uint8> headerToBytes = Conv_IntToBytes(header);
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
	//�������� Ÿ��
	int16 type = (int16)EProtocolType::PlayerMoveAck;
	TArray<uint8> typeToBytes = Conv_Int16ToBytes(type);
	temp.Append(typeToBytes);

	temp.Append(data);
	//��ü ���� ����
	int32 header = temp.Num() + 4;
	TArray<uint8> headerToBytes = Conv_IntToBytes(header);
	buffer.Append(headerToBytes);
	buffer.Append(temp);
	SendData(0, buffer);
}

void AGameTcpSocketConnection::SendNickname(const FString& message)
{
	UE_LOG(LogTemp, Log, TEXT("Log: SendNickname"));

	FPacketSetNicknameReq packet;

	packet.UserName = TCHAR_TO_UTF8(*message);
	
	FBufferArchive archive(true);
	FPacketSetNicknameReq::StaticStruct()->SerializeBin(archive, &packet);

	TArray<uint8> data = archive;
	TArray<uint8> buffer;
	TArray<uint8> temp;
	//�������� Ÿ��
	int16 type = (int16)EProtocolType::SetNicknameAck;
	TArray<uint8> typeToBytes = Conv_Int16ToBytes(type);
	temp.Append(typeToBytes);

	temp.Append(data);
	//��ü ���� ����
	int32 header = temp.Num() + 4;
	TArray<uint8> headerToBytes = Conv_IntToBytes(header);
	buffer.Append(headerToBytes);
	buffer.Append(temp);
	SendData(0, buffer);
}

