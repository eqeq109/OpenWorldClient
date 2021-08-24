// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OpenWorldClientGameMode.generated.h"

UCLASS(minimalapi)
class AOpenWorldClientGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOpenWorldClientGameMode();
	UFUNCTION(BlueprintCallable, Category = "UMG_Game")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);
	UFUNCTION(BlueprintCallable, Category = "Game")
		void SendNickname(const FString& nick);

	UFUNCTION()
		void OnSendNicknameAck(const FString& nick);
	

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
		TSubclassOf<UUserWidget> StartingWidgetClass;

	UPROPERTY()
		UUserWidget* CurrentWidget;
	
};



