#pragma once

#include "CoreMinimal.h"
#include "MultiplayerFPS/Default/MultiplayerFPSGameState.h"
#include "DM_GameState.generated.h"

UCLASS()
class MULTIPLAYERFPS_API ADM_GameState : public AMultiplayerFPSGameState
{
	GENERATED_BODY()

public:
	ADM_GameState();

	virtual void BeginPlay() override;

	virtual void GameEnded() override;

};
