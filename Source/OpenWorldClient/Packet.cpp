//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "Packet.h"
//
//Packet::Packet()
//{
//	Buffer = new TArray<uint8>();
//}
//
//Packet* Packet::Create(int16 type)
//{
//	Packet *packet = new Packet();
//	packet->SetProtocol(type);
//	return packet;
//}
//
//Packet::Packet(TArray<uint8>* buffer)
//{
//	Buffer = buffer;
//	Position = 4;
//	Size = buffer->GetAllocatedSize();
//
//	ProtocolType = PopProtocolType();
//}
//
//int16 Packet::PopProtocolType()
//{
//	return PopInt16();
//}
//
////uint8 Packet::PopByte()
////{
////	/*uint8 data = Buffer[Position];
////	Position += sizeof(uint8);
////	return data;*/
////}
//
//Packet::~Packet()
//{
//	delete(Buffer);
//}
