// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "MainUIBase.h"
#include "UObject/ConstructorHelpers.h"

AMainHUD::AMainHUD()
{
	static ConstructorHelpers::FClassFinder<UMainUIBase> MainUI(TEXT("WidgetBlueprint'/Game/UI/MainUI.MainUI_C'"));
	if (MainUI.Succeeded())
	{
		mMainClass = MainUI.Class;
	}
}

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	CheckUIObject();
}

TSharedPtr<SWidget> AMainHUD::GetChatInputTextObject()
{
	return mMainUIObject->GetChatInputTextObject();
}

void AMainHUD::AddChatMessage(const FString& message)
{
	if (!CheckUIObject()) return;

	mMainUIObject->AddChatMessage(message);
}

bool AMainHUD::CheckUIObject()
{
	if (mMainUIObject == nullptr)
	{
		return CreateUIObject();
	}
	return true;
}

bool AMainHUD::CreateUIObject()
{
	if (mMainClass)
	{
		mMainUIObject = CreateWidget<UMainUIBase>(GetOwningPlayerController(), mMainClass);
		if (mMainUIObject)
		{
			mMainUIObject->AddToViewport();
			return true;
		}
	}
	return false;
}