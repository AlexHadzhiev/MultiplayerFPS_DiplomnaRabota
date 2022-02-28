// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DM_LeaderBoardWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_MULTIPLAY_API UDM_LeaderBoardWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UDM_LeaderBoardWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	void UpdateStats(int Kills, int Deaths);
	void ResetStats();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TXTBlock_Kills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TXTBlock_Deaths;

};
