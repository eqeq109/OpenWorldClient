// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatUIBase.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDCLIENT_API UChatUIBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void AddChatMessage(const FString& message);
	FText GetChatInputTextMessage();
	TSharedPtr<class SWidget> GetChatInputTextObject();
	void SetChatInputTextMessage(const FText& text);

private:
	UPROPERTY(Meta = (BindWidget))
		class UScrollBox* ChatHistoryArea;

	UPROPERTY(Meta = (BindWidget))
		class UEditableTextBox* ChatInputText;

	UFUNCTION()
		void OnChatTextCommitted(const FText& text, ETextCommit::Type commitMethod);
};


