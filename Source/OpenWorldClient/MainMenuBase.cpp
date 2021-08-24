// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuBase.h"
#include "ChatUIBase.h"

TSharedPtr<SWidget> UMainMenuBase::GetChatInputTextObject()
{
	return mChatUI->GetChatInputTextObject();
}

void UMainMenuBase::AddChatMessage(const FString& message)
{
	mChatUI->AddChatMessage(message);
}
