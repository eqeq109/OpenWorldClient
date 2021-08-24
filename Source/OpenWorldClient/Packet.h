//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//
///**
// * 
// */
//class OPENWORLDCLIENT_API Packet
//{
//public:
//	Packet();
//	Packet(TArray<uint8>* buffer);
//	~Packet();
//
//	static Packet* Create(int16 type);
//	int16 PopProtocolType();
//	void CopyTo(Packet* target);
//	void OverWrite(TArray<uint8>* source, int position);
//	uint8 PopByte();
//	int16 PopInt16();
//	int32 PopInt32();
//	FString PopString();
//	float PopFloat();
//	void SetProtocol(int16 type);
//	void RecordSize();
//	void PushInt16(int16 data);
//	void Push(uint8 data);
//	void Push(int16 data);
//	void Push(int32 data);
//	void Push(FString data);
//	void Push(float data);
//
//	TArray<uint8>* Buffer;
//	int Position;
//	int Size;
//	int16 ProtocolType;
//};
