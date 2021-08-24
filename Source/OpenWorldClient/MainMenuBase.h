// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuBase.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDCLIENT_API UMainMenuBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	TSharedPtr<class SWidget> GetChatInputTextObject();
	void AddChatMessage(const FString& message);

private:
	UPROPERTY(Meta = (BindWidget))
		class UChatUIBase* mChatUI;
};
