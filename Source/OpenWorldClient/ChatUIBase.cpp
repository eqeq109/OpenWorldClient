// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatUIBase.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include <OpenWorldClient/MainControllerPC.h>
#include "Kismet/GameplayStatics.h"

void UChatUIBase::NativeConstruct()
{
	Super::NativeConstruct();

	ChatInputText->OnTextCommitted.AddDynamic(this, &UChatUIBase::OnChatTextCommitted);
}

void UChatUIBase::AddChatMessage(const FString& message)
{
	//text 오브젝트를 생성, ScrollBox에 추가
	UTextBlock* newTextBlock = NewObject<UTextBlock>(ChatHistoryArea);
	newTextBlock->SetText(FText::FromString(message));

	ChatHistoryArea->AddChild(newTextBlock);
	ChatHistoryArea->ScrollToEnd();
}

void UChatUIBase::SetChatInputTextMessage(const FText& text)
{
	ChatInputText->SetText(text);
}

TSharedPtr<SWidget> UChatUIBase::GetChatInputTextObject()
{
	return ChatInputText->GetCachedWidget();
}

void UChatUIBase::OnChatTextCommitted(const FText& text, ETextCommit::Type commitMethod)
{
	AMainControllerPC* myPC = Cast<AMainControllerPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (myPC == nullptr) return;

	switch (commitMethod)
	{
	case ETextCommit::OnEnter:
		if (text.IsEmpty() == false)
		{
			myPC->SendMessage(text);
			SetChatInputTextMessage(FText::GetEmpty());
		}
		myPC->FocusGame();
		break;
	case ETextCommit::OnCleared:
		myPC->FocusGame();
		break;
	}
}