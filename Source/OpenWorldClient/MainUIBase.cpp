// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUIBase.h"
#include "ChatUIBase.h"

TSharedPtr<SWidget> UMainUIBase::GetChatInputTextObject()
{
	return mChatUI->GetChatInputTextObject();
}

void UMainUIBase::AddChatMessage(const FString& message)
{
	mChatUI->AddChatMessage(message);
}