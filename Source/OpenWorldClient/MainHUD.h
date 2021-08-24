// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDCLIENT_API AMainHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AMainHUD();
	virtual void BeginPlay() override;

	TSharedPtr<class SWidget> GetChatInputTextObject();
	void AddChatMessage(const FString& message);

private:
	TSubclassOf<class UMainUIBase> mMainClass;
	class UMainUIBase* mMainUIObject;
	
	bool CheckUIObject();
	bool CreateUIObject();
};
