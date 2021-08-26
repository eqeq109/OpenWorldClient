// Fill out your copyright notice in the Description page of Project Settings.


#include "TcpSocketConnection.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "IpAddress.h"
#include "Sockets.h"
#include "HAL/RunnableThread.h"
#include "Async/Async.h"
#include <string>
#include "Logging/MessageLog.h"
#include "HAL/UnrealMemory.h"
#include "TcpSocketSettings.h"
#include <Runtime/CoreUObject/Public/Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <Runtime/JsonUtilities/Public/JsonObjectConverter.h>
//#include <Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h>

// Sets default values
ATcpSocketConnection::ATcpSocketConnection()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATcpSocketConnection::BeginPlay()
{
	Super::BeginPlay();
}

void ATcpSocketConnection::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TArray<int32> keys;
	mTcpWorkers.GetKeys(keys);

	for (auto& key : keys)
	{
		Disconnect(key);
	}
}

// Called every frame
void ATcpSocketConnection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATcpSocketConnection::Connect(const FString& mIpAddress, int32 port, /*const FTcpSocketDisconnectDelegate& OnDisconnected, const FTcpSocketConnectDelegate& OnConnected,
	const FTcpSocketReceivedMessageDelegate& OnMessageReceived,*/ int32& connectionID)
{
	//DisconnectedDelegate = OnDisconnected;
	//ConnectedDelegate = OnConnected;
	//MessageReceivedDelegate = OnMessageReceived;

	connectionID = mTcpWorkers.Num();

	TWeakObjectPtr<ATcpSocketConnection> thisWeakObjPtr = TWeakObjectPtr<ATcpSocketConnection>(this);
	TSharedRef<FTcpSocketWorker> worker(new FTcpSocketWorker(mIpAddress, port, thisWeakObjPtr, connectionID, ReceiveBufferSize, SendBufferSize, TimeBetweenTicks));
	mTcpWorkers.Add(connectionID, worker);
	worker->Start();
}

void ATcpSocketConnection::Disconnect(int32 connectionID)
{
	auto worker = mTcpWorkers.Find(connectionID);
	if (worker)
	{
		UE_LOG(LogTemp, Log, TEXT("Tcp Socket: Disconnected from server."));
		worker->Get().Stop();
		mTcpWorkers.Remove(connectionID);
	}
}

bool ATcpSocketConnection::SendData(int32 connectionID /*= 0*/, TArray<uint8> dataToSend)
{
	if (mTcpWorkers.Contains(connectionID))
	{
		if (mTcpWorkers[connectionID]->isConnected())
		{
			mTcpWorkers[connectionID]->AddToOutbox(dataToSend);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Log: Socket %d isn't connected"), connectionID);
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Log: SocketId %d doesn't exist"), connectionID);
	}
	return false;
}

void ATcpSocketConnection::ExecuteOnMessageReceived(int32 connectionID, TWeakObjectPtr<ATcpSocketConnection> thisObj)
{
	// the second check is for when we quit PIE, we may get a message about a disconnect, but it's too late to act on it, because the thread has already been killed
	if (!thisObj.IsValid())
		return;

	// how to crash:
	// 1 connect with both clients
	// 2 stop PIE
	// 3 close editor
	if (!mTcpWorkers.Contains(connectionID)) {
		return;
	}

	TArray<uint8> msg = mTcpWorkers[connectionID]->ReadFromInbox();
	bool isBound = MessageReceivedDelegate.IsBound();
	MessageReceivedDelegate.ExecuteIfBound(connectionID, msg);
}

TArray<uint8> ATcpSocketConnection::ConcatBytesBytes(TArray<uint8> A, TArray<uint8 > B)
{
	TArray<uint8> ArrayResult;

	for (int i = 0; i < A.Num(); i++)
	{
		ArrayResult.Add(A[i]);
	}

	for (int i = 0; i < B.Num(); i++)
	{
		ArrayResult.Add(B[i]);
	}

	return ArrayResult;
}

TArray<uint8> ATcpSocketConnection::ConvIntToBytes(int32 InInt)
{
	TArray<uint8> result;
	for (int i = 0; i < 4; i++)
	{
		result.Add(InInt >> i * 8);
	}
	return result;
}

TArray<uint8> ATcpSocketConnection::ConvInt16ToBytes(int16 InInt)
{
	TArray<uint8> result;
	for (int i = 0; i < 2; i++)
	{
		result.Add(InInt >> i * 8);
	}
	return result;
}

TArray<uint8> ATcpSocketConnection::ConvStringToBytes(const FString& InStr)
{
	FTCHARToUTF8 Convert(*InStr);
	int BytesLength = Convert.Length(); //length of the utf-8 string in bytes (when non-latin letters are used, it's longer than just the number of characters)
	uint8* messageBytes = static_cast<uint8*>(FMemory::Malloc(BytesLength));
	FMemory::Memcpy(messageBytes, (uint8*)TCHAR_TO_UTF8(InStr.GetCharArray().GetData()), BytesLength); //mcmpy is required, since TCHAR_TO_UTF8 returns an object with a very short lifetime

	TArray<uint8> result;
	for (int i = 0; i < BytesLength; i++)
	{
		result.Add(messageBytes[i]);
	}

	FMemory::Free(messageBytes);

	return result;
}

TArray<uint8> ATcpSocketConnection::ConvFloatToBytes(float InFloat)
{
	TArray<uint8> result;

	unsigned char const* p = reinterpret_cast<unsigned char const*>(&InFloat);
	for (int i = 0; i != sizeof(float); i++)
	{
		result.Add((uint8)p[i]);
	}
	return result;
}

TArray<uint8> ATcpSocketConnection::ConvByteToBytes(uint8 inByte)
{
	TArray<uint8> result{ inByte };
	return result;
}

int32 ATcpSocketConnection::PopInt(TArray<uint8>& Message)
{
	if (Message.Num() < 4)
	{
		PrintToConsole("Error in the ReadInt node. Not enough bytes in the Message.", true);
		return -1;
	}

	int result;
	unsigned char byteArray[4];

	for (int i = 0; i < 4; i++)
	{
		byteArray[i] = Message[0];
		Message.RemoveAt(0);
	}

	FMemory::Memcpy(&result, byteArray, 4);

	return result;
}

int16 ATcpSocketConnection::PopInt16(TArray<uint8>& Message)
{
	if (Message.Num() < 2)
	{
		PrintToConsole("Error in the ReadInt node. Not enough bytes in the Message.", true);
		return -1;
	}

	int result;
	unsigned char byteArray[2];

	for (int i = 0; i < 2; i++)
	{
		byteArray[i] = Message[0];
		Message.RemoveAt(0);
	}

	FMemory::Memcpy(&result, byteArray, 2);

	return result;
}

uint8 ATcpSocketConnection::PopByte(TArray<uint8>& Message)
{
	if (Message.Num() < 1)
	{
		PrintToConsole("Error in the ReadByte node. Not enough bytes in the Message.", true);
		return 255;
	}

	uint8 result = Message[0];
	Message.RemoveAt(0);
	return result;
}

bool ATcpSocketConnection::PopBytes(int32 NumBytes, TArray<uint8>& Message, TArray<uint8>& returnArray)
{
	if (Message.Num() <= 0)
		return false;
	for (int i = 0; i < NumBytes; i++) {
		if (Message.Num() >= 1)
			returnArray.Add(PopByte(Message));
		//else
		//	return false;
	}
	return true;
}

float ATcpSocketConnection::PopFloat(TArray<uint8>& Message)
{
	if (Message.Num() < 4)
	{
		PrintToConsole("Error in the ReadFloat node. Not enough bytes in the Message.", true);
		return -1.f;
	}

	float result;
	unsigned char byteArray[4];

	for (int i = 0; i < 4; i++)
	{
		byteArray[i] = Message[0];
		Message.RemoveAt(0);
	}

	FMemory::Memcpy(&result, byteArray, 4);

	return result;
}

FString ATcpSocketConnection::PopString(TArray<uint8>& Message, int32 BytesLength)
{
	if (BytesLength <= 0)
	{
		if (BytesLength < 0)
			PrintToConsole("Error in the ReadString node. BytesLength isn't a positive number.", true);
		return FString("");
	}
	if (Message.Num() < BytesLength)
	{
		PrintToConsole("Error in the ReadString node. Message isn't as long as BytesLength.", true);
		return FString("");
	}

	TArray<uint8> StringAsArray;
	StringAsArray.Reserve(BytesLength);

	for (int i = 0; i < BytesLength; i++)
	{
		StringAsArray.Add(Message[0]);
		Message.RemoveAt(0);
	}

	std::string cstr(reinterpret_cast<const char*>(StringAsArray.GetData()), StringAsArray.Num());
	return FString(UTF8_TO_TCHAR(cstr.c_str()));
}

TArray<uint8> ATcpSocketConnection::SerializeObject(UObject* target)
{
	TArray<uint8> bytes;
	FMemoryWriter memoryWriter(bytes, true);
	FObjectAndNameAsStringProxyArchive ar(memoryWriter, false);
	target->Serialize(ar);

	return bytes;
}

void ATcpSocketConnection::DeserializeObject(UObject* target, TArray<uint8> bytes)
{
	FMemoryReader memoryReader(bytes, true);
	FObjectAndNameAsStringProxyArchive ar(memoryReader, false);
	target->Serialize(ar);
}
template<typename InStructType>
FString& ATcpSocketConnection::SerializeStructToJson(const InStructType& object)
{
	FString retJson;

	FJsonObjectConverter::UStructToJsonObjectString(object, retJson, 0, 0);

	return retJson;
}
template<typename InStructType>
void ATcpSocketConnection::DeserializeJsonToStruct(InStructType* target, const FString& json)
{
	FJsonObjectConverter::JsonObjectStringToUStruct(json, target, 0, 0);
}

bool ATcpSocketConnection::IsConnected(int32 connectionID)
{
	if (mTcpWorkers.Contains(connectionID))
		return mTcpWorkers[connectionID]->isConnected();
	return false;
}

void ATcpSocketConnection::PrintToConsole(FString Str, bool Error)
{
	if (auto tcpSocketSettings = GetDefault<UTcpSocketSettings>())
	{
		if (Error && tcpSocketSettings->bPostErrorsToMessageLog)
		{
			auto messageLog = FMessageLog("Tcp Socket Plugin");
			messageLog.Open(EMessageSeverity::Error, true);
			messageLog.Message(EMessageSeverity::Error, FText::AsCultureInvariant(Str));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Log: %s"), *Str);
		}
	}
}

void ATcpSocketConnection::ExecuteOnConnected(int32 WorkerId, TWeakObjectPtr<ATcpSocketConnection> thisObj)
{
	if (!thisObj.IsValid())
		return;

	ConnectedDelegate.ExecuteIfBound(WorkerId);
}

void ATcpSocketConnection::ExecuteOnDisconnected(int32 WorkerId, TWeakObjectPtr<ATcpSocketConnection> thisObj)
{
	if (!thisObj.IsValid())
		return;

	if (mTcpWorkers.Contains(WorkerId))
	{
		mTcpWorkers.Remove(WorkerId);
	}
	DisconnectedDelegate.ExecuteIfBound(WorkerId);
}

bool FTcpSocketWorker::isConnected()
{
	///FScopeLock ScopeLock(&SendCriticalSection);
	return mIsConnected;
}

FTcpSocketWorker::FTcpSocketWorker(FString inIp, const int32 inPort, TWeakObjectPtr<ATcpSocketConnection> InOwner, int32 inId, int32 inRecvBufferSize, int32 inSendBufferSize, float inTimeBetweenTicks)
	: mIpAddress(inIp)
	, mPort(inPort)
	, mThreadSpawnerActor(InOwner)
	, mID(inId)
	, mRecvBufferSize(inRecvBufferSize)
	, mSendBufferSize(inSendBufferSize)
	, mTimeBetweenTicks(inTimeBetweenTicks)
{

}

FTcpSocketWorker::~FTcpSocketWorker()
{
	AsyncTask(ENamedThreads::GameThread, []() {	ATcpSocketConnection::PrintToConsole("Tcp socket thread was destroyed.", false); });
	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
}

void FTcpSocketWorker::Start()
{
	check(!Thread && "Thread wasn't null at the start!");
	check(FPlatformProcess::SupportsMultithreading() && "This platform doesn't support multithreading!");
	if (Thread)
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Thread isn't null. It's: %s"), *Thread->GetThreadName());
	}
	Thread = FRunnableThread::Create(this, *FString::Printf(TEXT("FTcpSocketWorker %s:%d"), *mIpAddress, mPort), 128 * 1024, TPri_Normal);
	UE_LOG(LogTemp, Log, TEXT("Log: Created thread"));
}

void FTcpSocketWorker::AddToOutbox(TArray<uint8> Message)
{
	mOutbox.Enqueue(Message);
}

TArray<uint8> FTcpSocketWorker::ReadFromInbox()
{
	TArray<uint8> msg;
	mInbox.Dequeue(msg);
	return msg;
}

bool FTcpSocketWorker::Init()
{
	mIsRun = true;
	mIsConnected = false;
	return true;
}

uint32 FTcpSocketWorker::Run()
{
	AsyncTask(ENamedThreads::GameThread, []() {	ATcpSocketConnection::PrintToConsole("Starting Tcp socket thread.", false); });

	while (mIsRun)
	{
		FDateTime timeBeginningOfTick = FDateTime::UtcNow();

		// Connect
		if (!mIsConnected)
		{
			mSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
			if (!mSocket)
			{
				return 0;
			}

			mSocket->SetReceiveBufferSize(mRecvBufferSize, mActualRecvBufferSize);
			mSocket->SetSendBufferSize(mSendBufferSize, mActualSendBufferSize);

			FIPv4Address ip;
			FIPv4Address::Parse(mIpAddress, ip);

			TSharedRef<FInternetAddr> internetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
			internetAddr->SetIp(ip.Value);
			internetAddr->SetPort(mPort);

			mIsConnected = mSocket->Connect(*internetAddr);
			if (mIsConnected)
			{
				AsyncTask(ENamedThreads::GameThread, [this]() {
					mThreadSpawnerActor.Get()->ExecuteOnConnected(mID, mThreadSpawnerActor);
					});
			}
			else
			{
				AsyncTask(ENamedThreads::GameThread, []() { ATcpSocketConnection::PrintToConsole(FString::Printf(TEXT("Couldn't connect to server. TcpSocketConnection.cpp: line %d"), __LINE__), true); });
				mIsRun = false;
			}
			continue;
		}

		if (!mSocket)
		{
			AsyncTask(ENamedThreads::GameThread, []() { ATcpSocketConnection::PrintToConsole(FString::Printf(TEXT("Socket is null. TcpSocketConnection.cpp: line %d"), __LINE__), true); });
			mIsRun = false;
			continue;
		}

		// check if we weren't disconnected from the socket
		mSocket->SetNonBlocking(true); // set to NonBlocking, because Blocking can't check for a disconnect for some reason
		int32 t_BytesRead;
		uint8 t_Dummy;
		if (!mSocket->Recv(&t_Dummy, 1, t_BytesRead, ESocketReceiveFlags::Peek))
		{
			mIsRun = false;
			continue;
		}
		mSocket->SetNonBlocking(false);	// set back to Blocking

		// if Outbox has something to send, send it
		while (!mOutbox.IsEmpty())
		{
			TArray<uint8> toSend;
			mOutbox.Dequeue(toSend);

			if (!BlockingSend(toSend.GetData(), toSend.Num()))
			{
				// if sending failed, stop running the thread
				mIsRun = false;
				UE_LOG(LogTemp, Log, TEXT("TCP send data failed !"));
				continue;
			}
		}

		// if we can read something		
		uint32 PendingDataSize = 0;
		TArray<uint8> receivedData;

		int32 BytesReadTotal = 0;
		// keep going until we have no data.
		while (mIsRun)
		{
			if (!mSocket->HasPendingData(PendingDataSize))
			{
				// no messages
				break;
			}

			AsyncTask(ENamedThreads::GameThread, []() { ATcpSocketConnection::PrintToConsole("Pending data", false); });

			receivedData.SetNumUninitialized(BytesReadTotal + PendingDataSize);

			int32 BytesRead = 0;
			if (!mSocket->Recv(receivedData.GetData() + BytesReadTotal, PendingDataSize, BytesRead))
			{
				// ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
				// error code: (int32)SocketSubsystem->GetLastErrorCode()
				AsyncTask(ENamedThreads::GameThread, []() {
					ATcpSocketConnection::PrintToConsole(FString::Printf(TEXT("In progress read failed. TcpSocketConnection.cpp: line %d"), __LINE__), true);
					});
				break;
			}
			BytesReadTotal += BytesRead;

			/* TODO: if we have more PendingData than we could read, continue the while loop so that we can send messages if we have any, and then keep recving*/
		}

		// if we received data, inform the main thread about it, so it can read TQueue
		if (mIsRun && receivedData.Num() != 0)
		{
			mInbox.Enqueue(receivedData);
			AsyncTask(ENamedThreads::GameThread, [this]() {
				mThreadSpawnerActor.Get()->ExecuteOnMessageReceived(mID, mThreadSpawnerActor);
				});
		}

		/* In order to sleep, we will account for how much this tick took due to sending and receiving */
		FDateTime timeEndOfTick = FDateTime::UtcNow();
		FTimespan tickDuration = timeEndOfTick - timeBeginningOfTick;
		float secondsThisTickTook = tickDuration.GetTotalSeconds();
		float timeToSleep = mTimeBetweenTicks - secondsThisTickTook;
		if (timeToSleep > 0.f)
		{
			//AsyncTask(ENamedThreads::GameThread, [timeToSleep]() { ATcpSocketConnection::PrintToConsole(FString::Printf(TEXT("Sleeping: %f seconds"), timeToSleep), false); });
			FPlatformProcess::Sleep(timeToSleep);
		}
	}

	mIsConnected = false;

	AsyncTask(ENamedThreads::GameThread, [this]() {
		mThreadSpawnerActor.Get()->ExecuteOnDisconnected(mID, mThreadSpawnerActor);
		});

	SocketShutdown();
	if (mSocket)
	{
		delete mSocket;
		mSocket = nullptr;
	}

	return 0;
}

void FTcpSocketWorker::Stop()
{
	mIsRun = false;
}

void FTcpSocketWorker::Exit()
{

}

bool FTcpSocketWorker::BlockingSend(const uint8* Data, int32 BytesToSend)
{
	if (BytesToSend > 0)
	{
		int32 BytesSent = 0;
		if (!mSocket->Send(Data, BytesToSend, BytesSent))
		{
			return false;
		}
	}
	return true;
}

void FTcpSocketWorker::SocketShutdown()
{
	// if there is still a socket, close it so our peer will get a quick disconnect notification
	if (mSocket)
	{
		mSocket->Close();
	}
}
