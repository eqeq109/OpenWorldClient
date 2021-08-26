// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "Containers/Queue.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "TcpSocketConnection.generated.h"

//DECLARE_DYNAMIC_DELEGATE_OneParam(FTcpSocketDisconnectDelegate, int32, connectionID);
//DECLARE_DYNAMIC_DELEGATE_OneParam(FTcpSocketConnectDelegate, int32, connectionID);
//DECLARE_DYNAMIC_DELEGATE_TwoParams(FTcpSocketReceivedMessageDelegate, int32,connectionID, TArray<uint8>&, message);

using FTcpSocketDisconnectDelegate = TDelegate<void(int32 connectionID)>;
using FTcpSocketConnectDelegate = TDelegate<void(int32 connectionID)>;
using FTcpSocketReceivedMessageDelegate = TDelegate<void(int32 connectionID, TArray<uint8>& message)>;

UCLASS(Blueprintable, BlueprintType)
class OPENWORLDCLIENT_API ATcpSocketConnection : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATcpSocketConnection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Returns the ID of the new connection. */
	UFUNCTION(BlueprintCallable, Category = "Socket")
		void Connect(const FString& ipAddress, int32 port,
			/*			const FTcpSocketDisconnectDelegate& OnDisconnected, const FTcpSocketConnectDelegate& OnConnected,
						const FTcpSocketReceivedMessageDelegate& OnMessageReceived, */int32& ConnectionId);

	/* Disconnect from connection ID. */
	UFUNCTION(BlueprintCallable, Category = "Socket")
		void Disconnect(int32 connectionId);

	/* False means we're not connected to socket and the data wasn't sent. "True" doesn't guarantee that it was successfully sent,
	only that we were still connected when we initiating the sending process. */
	UFUNCTION(BlueprintCallable, Category = "Socket") // use meta to set first default param to 0
		bool SendData(int32 connectionId, TArray<uint8> dataToSend);

	/*
	When hitting Stop in PIE while a connection is being established (it's a blocking operation that takes a while to timeout),
	our ATcpSocketConnection actor will be destroyed, an then the thread will send a message through AsyncTask to call ExecuteOnConnected,
	ExecuteOnDisconnected, or ExecuteOnMessageReceived.
	When we enter their code, "this" will point to random memory.
	So to avoid that problem, we also send back a weak pointer as well. If the pointer is valid, we're ok.
	This is why the three methods below have a TWeakObjectPtr.
	*/

	//UFUNCTION(Category = "Socket")	
	void ExecuteOnConnected(int32 workerId, TWeakObjectPtr<ATcpSocketConnection> thisObj);

	//UFUNCTION(Category = "Socket")
	void ExecuteOnDisconnected(int32 workerId, TWeakObjectPtr<ATcpSocketConnection> thisObj);

	//UFUNCTION(Category = "Socket")
	void ExecuteOnMessageReceived(int32 connectionId, TWeakObjectPtr<ATcpSocketConnection> thisObj);

	/*UFUNCTION(BlueprintPure, meta = (DisplayName = "Append Bytes", CommutativeAssociativeBinaryOperator = "true"), Category = "Socket")
	static TArray<uint8> Concat_BytesBytes(const TArray<uint8>& A, const TArray<uint8>& B);*/

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Append Bytes", CommutativeAssociativeBinaryOperator = "true"), Category = "Socket")
		static TArray<uint8> ConcatBytesBytes(TArray<uint8> a, TArray<uint8> b);

	/** Converts an integer to an array of bytes */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Int To Bytes", CompactNodeTitle = "->", Keywords = "cast convert", BlueprintAutocast), Category = "Socket")
		static TArray<uint8> ConvIntToBytes(int32 inInt);

	/** Converts an integer to an array of bytes */
	UFUNCTION()
		static TArray<uint8> ConvInt16ToBytes(int16 inInt);

	/** Converts a string to an array of bytes */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Bytes", CompactNodeTitle = "->", Keywords = "cast convert", BlueprintAutocast), Category = "Socket")
		static TArray<uint8> ConvStringToBytes(const FString& inStr);

	/** Converts a float to an array of bytes */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Float To Bytes", CompactNodeTitle = "->", Keywords = "cast convert", BlueprintAutocast), Category = "Socket")
		static TArray<uint8> ConvFloatToBytes(float inFloat);

	/** Converts a byte to an array of bytes */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Byte To Bytes", CompactNodeTitle = "->", Keywords = "cast convert", BlueprintAutocast), Category = "Socket")
		static TArray<uint8> ConvByteToBytes(uint8 inByte);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Int", Keywords = "read int"), Category = "Socket")
		static int32 PopInt(UPARAM(ref) TArray<uint8>& message);

	UFUNCTION()
		static int16 PopInt16(UPARAM(ref) TArray<uint8>& message);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Byte", Keywords = "read byte int8 uint8"), Category = "Socket")
		static uint8 PopByte(UPARAM(ref) TArray<uint8>& message);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Bytes", Keywords = "read bytes"), Category = "Socket")
		static bool PopBytes(int32 NumBytes, UPARAM(ref) TArray<uint8>& Message, TArray<uint8>& ReturnArray);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Float", Keywords = "read float"), Category = "Socket")
		static float PopFloat(UPARAM(ref) TArray<uint8>& message);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read String", Keywords = "read string"), Category = "Socket")
		static FString PopString(UPARAM(ref) TArray<uint8>& message, int32 stringLength);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Socket")
		bool IsConnected(int32 connectionId);

	UFUNCTION()
		TArray<uint8> SerializeObject(UObject* source);
	UFUNCTION()
		void DeserializeObject(UObject* target, TArray<uint8> source);
	//Not use
	template<typename InStructType>
		FString& SerializeStructToJson(const InStructType& object);
	//Not use
	template<typename InStructType>
		void DeserializeJsonToStruct(InStructType* target, const FString& json);

	/* Used by the separate threads to print to console on the main thread. */
	static void PrintToConsole(FString Str, bool Error);

	/* Buffer size in bytes. Currently not used. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Socket")
		int32 SendBufferSize = 16384;

	/* Buffer size in bytes. It's set only when creating a socket, never afterwards. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Socket")
		int32 ReceiveBufferSize = 16384;

	/* Time between ticks. Please account for the fact that it takes 1ms to wake up on a modern PC, so 0.01f would effectively be 0.011f */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Socket")
		float TimeBetweenTicks = 0.008f;

public:
	//종료 요청. S -> C
	const short SYS_CLOSE_REQ = 0;
	//종료 응답. C -> S
	const short SYS_CLOSE_ACK = -1;
	//하트비트 시작. S -> C
	const short SYS_START_HEARTBEAT = -2;
	//하트비트 갱신. C -> S
	const short SYS_UPDATE_HEARTBEAT = -3;

private:
	TMap<int32, TSharedRef<class FTcpSocketWorker>> mTcpWorkers;

public:
	FTcpSocketDisconnectDelegate DisconnectedDelegate;
	FTcpSocketConnectDelegate ConnectedDelegate;
	FTcpSocketReceivedMessageDelegate MessageReceivedDelegate;
};

class FTcpSocketWorker : public FRunnable, public TSharedFromThis<FTcpSocketWorker>
{

	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread = nullptr;

private:
	class FSocket* mSocket = nullptr;
	FString mIpAddress;
	int mPort;
	TWeakObjectPtr<ATcpSocketConnection> mThreadSpawnerActor;
	int32 mID;
	int32 mRecvBufferSize;
	int32 mActualRecvBufferSize;
	int32 mSendBufferSize;
	int32 mActualSendBufferSize;
	float mTimeBetweenTicks;
	FThreadSafeBool mIsConnected = false;

	// SPSC = single producer, single consumer.
	TQueue<TArray<uint8>, EQueueMode::Spsc> mInbox; // Messages we read from socket and send to main thread. Runner thread is producer, main thread is consumer.
	TQueue<TArray<uint8>, EQueueMode::Spsc> mOutbox; // Messages to send to socket from main thread. Main thread is producer, runner thread is consumer.

public:

	//Constructor / Destructor
	FTcpSocketWorker(FString inIp, const int32 inPort, TWeakObjectPtr<ATcpSocketConnection> inOwner, int32 inId, int32 inRecvBufferSize, int32 inSendBufferSize, float inTimeBetweenTicks);
	virtual ~FTcpSocketWorker();

	/*  Starts processing of the connection. Needs to be called immediately after construction	 */
	void Start();

	/* Adds a message to the outgoing message queue */
	void AddToOutbox(TArray<uint8> message);

	/* Reads a message from the inbox queue */
	TArray<uint8> ReadFromInbox();

	// Begin FRunnable interface.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	// End FRunnable interface	

	/** Shuts down the thread */
	void SocketShutdown();

	/* Getter for bConnected */
	bool isConnected();

private:
	/* Blocking send */
	bool BlockingSend(const uint8* data, int32 bytesToSend);

	/** thread should continue running */
	FThreadSafeBool mIsRun = false;

	/** Critical section preventing multiple threads from sending simultaneously */
	//FCriticalSection SendCriticalSection;
};

