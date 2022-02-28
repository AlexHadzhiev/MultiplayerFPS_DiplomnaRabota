// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DM_ObjectiveStatsWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_MULTIPLAY_API UDM_ObjectiveStatsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDM_ObjectiveStatsWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	void UpdateStats();
	void ResetStats();

};
