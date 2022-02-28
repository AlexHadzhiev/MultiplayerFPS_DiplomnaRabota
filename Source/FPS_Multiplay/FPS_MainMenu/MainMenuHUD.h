// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/WidgetComponent.h"
#include "MainMenuHUD.generated.h"

/**
 * 
 */
UCLASS()
class FPS_MULTIPLAY_API AMainMenuHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMainMenuHUD();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void DrawHUD() override;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY()
		class UMainMenuWidget* MainMenuWidget;
};
