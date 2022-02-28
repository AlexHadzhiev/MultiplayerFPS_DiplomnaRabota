// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_Flag.h"

#include "Kismet/GameplayStatics.h"
#include "FPS_Multiplay/CTF_GameMode/CTF_PlayerState.h"
#include "FPS_Multiplay/CTF_GameMode/CTF_Character.h"
#include "FPS_Multiplay/CTF_GameMode/CTF_GameMode.h"
#include "FPS_Multiplay/CTF_GameMode/CTF_GameState.h"

// Sets default values
AFPS_Flag::AFPS_Flag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent >(TEXT("Flag's Mesh"));
	RootComponent = MeshComponent;

	bIsDropped = false;
	RespawnTime = 3;
}

// Called when the game starts or when spawned
void AFPS_Flag::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AFPS_Flag::OnBeginOverlap);
	UWorld* World = GetWorld();
	if(IsValid(World))
	{
		ACTF_GameState* GameState = Cast<ACTF_GameState>(World->GetGameState());
		if (IsValid(GameState))
		{
			RespawnTime = GameState->FlagRespawnTime;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AFPS_Flag::BeginPlay() -> GameState is not Valid !!!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AFPS_Flag::BeginPlay() -> World is not Valid !!!"));
	}
}

// Called every frame
void AFPS_Flag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bIsDropped)
	{
		bIsDropped = false;
		GetWorldTimerManager().SetTimer(RespawnHandle, this, &AFPS_Flag::ServerRPCRespawnFlag, RespawnTime, false);
	}
}

void AFPS_Flag::OnBeginOverlap(class UPrimitiveComponent* HitComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACTF_Character* Player = Cast<ACTF_Character>(OtherActor);
	if (IsValid(Player))
	{

		// TODO: The function is called 2 times when it need 1 when overlapping!!! //
//static int a = 0;
//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT("Flag hitted! %d"), ++a ));
//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT("Actor: %s"), *Player->GetName()));

		TEnumAsByte<EFPS_Teams> PlayerTeam = Player->Team;
		if (PlayerTeam != TEAM_NONE)
		{
			ServerRPCFlagOverlappedLogic(Player);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FlagOverlap() ->PlayerTeam is NONE!!!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FlagOverlap() -> Player is not Valid!!!"));
	}
}

void AFPS_Flag::ServerRPCRespawnFlag_Implementation()
{
	if(HasAuthority())
	{
		UWorld* World = GetWorld();
		if(IsValid(World))
		{
			ACTF_GameMode* GameMode = Cast<ACTF_GameMode>(World->GetAuthGameMode());
			if (IsValid(GameMode))
			{
				Destroy(true);
				FTransform FlagTransform;
				TSubclassOf<AFPS_Flag> BluePrintClass;
				FActorSpawnParameters SpawnParameters;

				if (FlagTeam == TEAM_RED)
				{
					BluePrintClass = GameMode->getRedFlagBP();
					FlagTransform = GameMode->getRedFlagTransform();
				}
				else if (FlagTeam == TEAM_BLUE)
				{
					BluePrintClass = GameMode->getBlueFlagBP();
					FlagTransform = GameMode->getBlueFlagTransform();
				}
				World->SpawnActor<AFPS_Flag>(BluePrintClass, FlagTransform.GetLocation(), FlagTransform.GetRotation().Rotator(), SpawnParameters);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AFPS_Flag::ServerRPCRespawnFlag_Implementation() -> GameMode is not Valid !!!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AFPS_Flag::ServerRPCRespawnFlag_Implementation() -> World is not Valid !!!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AFPS_Flag::ServerRPCRespawnFlag_Implementation() -> Does not have Authority !!!"));
	}
}

void AFPS_Flag::ServerRPCFlagOverlappedLogic_Implementation(AActor* OtherActor)
{
	if (HasAuthority())
	{
		UWorld* World = GetWorld();
		if (IsValid(World))
		{
			ACTF_GameMode* GameMode = Cast<ACTF_GameMode>(World->GetAuthGameMode());
			if (IsValid(GameMode))
			{
				ACTF_GameState* GameState = Cast<ACTF_GameState>(GameMode->GameState);
				if (IsValid(GameState))
				{
					ACTF_Character* Player = Cast<ACTF_Character>(OtherActor);
					if (IsValid(Player))
					{
						ACTF_PlayerState* PlayerState = Cast<ACTF_PlayerState>(Player->GetPlayerState());
						if (IsValid(PlayerState))
						{
							TEnumAsByte<EFPS_Teams> PlayerTeam = Player->Team;
							if (FlagTeam != PlayerTeam)
							{
								if (!GetWorldTimerManager().IsTimerActive(RespawnHandle))
								{
									FTransform FlagTransform = GetActorTransform();

									if (FlagTeam == TEAM_NONE) UE_LOG(LogTemp, Error, TEXT("FlagOverlappedLogic_Implementation() -> FLAG TEAM WAS NOT SET!!!"));
									if (FlagTeam == TEAM_RED) GameMode->setRedFlagTransform(FlagTransform);
									if (FlagTeam == TEAM_BLUE) GameMode->setBlueFlagTransform(FlagTransform);
								}
								GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT("Player: %s has gotten the flag!"), *Player->GetName()));
								Player->bHasFlag = true;
								Destroy(true);
							}
							else if (Player->bHasFlag)
							{
								Player->bHasFlag = false;
								FTransform FlagTransform;
								TSubclassOf<AFPS_Flag> BluePrintClass;
								FActorSpawnParameters SpawnParameters;

								if (PlayerTeam == TEAM_BLUE)
								{
									BluePrintClass = GameMode->getRedFlagBP();
									FlagTransform = GameMode->getRedFlagTransform();
									GameState->BlueFlagCaptured();
								}
								else if(PlayerTeam == TEAM_RED)
								{
									BluePrintClass = GameMode->getBlueFlagBP();
									FlagTransform = GameMode->getBlueFlagTransform();
									GameState->RedFlagCaptured();
								}
								PlayerState->FlagWasCaptured();
								World->SpawnActor<AFPS_Flag>(BluePrintClass, FlagTransform.GetLocation(), FlagTransform.GetRotation().Rotator(), SpawnParameters);
								GameMode->UpdateObjectiveStats();
							}
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("FlagOverlappedLogic_Implementation() -> PlayerState is not Valid !!!"));
						}
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("FlagOverlappedLogic_Implementation() -> Player is not Valid !!!"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("FlagOverlappedLogic_Implementation() -> GameState is not Valid !!!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("FlagOverlappedLogic_Implementation() -> GameMode is not Valid !!!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FlagOverlappedLogic_Implementation() -> World is not Valid !!!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FlagOverlappedLogic_Implementation() -> Does not have Authority !!!"));
	}
}


