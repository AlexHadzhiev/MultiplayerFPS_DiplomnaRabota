#include "CTF_Character.h"
#include "CTF_PlayerController.h"
#include "CTF_GameMode.h"
#include "MultiplayerFPS/Default/MultiplayerFPSGameInstance.h"
#include "Net/UnrealNetwork.h"

ACTF_Character::ACTF_Character()
{
	bHasFlag = false;
}

void ACTF_Character::BeginPlay()
{
	Super::BeginPlay();
}

void ACTF_Character::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACTF_Character, bHasFlag);
}

void ACTF_Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ACTF_Character::ServerOnPlayerDeath()
{
	if (bHasFlag)
	{
		ServerDropFlag();
	}
	Super::ServerOnPlayerDeath();
}

void ACTF_Character::ServerDropFlag_Implementation()
{
	if(!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("ACTF_Character::ServerDropFlag_Implementation() -> Does not have Authority !!!"));
		return;
	}

	bHasFlag = false;

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("ACTF_Character::ServerDropFlag_Implementation() -> World is not Valid !!!"));
		return;
	}

	ACTF_GameMode* GameMode = Cast<ACTF_GameMode>(World->GetAuthGameMode());
	if (!IsValid(GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("ACTF_Character::ServerDropFlag_Implementation() -> GameMode is not Valid !!!"));
		return;
	}

	FTransform FlagTransform;
	TSubclassOf<AFlag> BluePrintClass;
	FActorSpawnParameters SpawnParameters;

	FlagTransform = GetActorTransform();

	if (Team == TEAM_BLUE)
	{
		BluePrintClass = GameMode->GetRedFlagBP();
	}
	else if (Team == TEAM_RED)
	{
		BluePrintClass = GameMode->GetBlueFlagBP();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ACTF_Character::ServerDropFlag_Implementation() -> Team is TEAM_NONE !!!"));
		return;
	}

	AFlag* Flag = World->SpawnActor<AFlag>(BluePrintClass, FlagTransform.GetLocation(), FlagTransform.GetRotation().Rotator(), SpawnParameters);
	Flag->bIsDropped = true;

}

