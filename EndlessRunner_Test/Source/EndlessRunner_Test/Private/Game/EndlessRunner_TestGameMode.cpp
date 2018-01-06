// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "EndlessRunner_Test.h"
#include "EndlessRunner_TestGameMode.h"
#include "EndlessRunner_TestCharacter.h"
#include "BaseTile.h"

#define print(position,text) if (GEngine) GEngine->AddOnScreenDebugMessage(position, 1.5, FColor::Red,text)//easy screen debug macro




AEndlessRunner_TestGameMode::AEndlessRunner_TestGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	
}


void AEndlessRunner_TestGameMode::SpawnTile()
{
	if (tileForwardCount >=7)
	{
		
			SpawnTurn();
		
	}
	else SpawnForward();
}


void AEndlessRunner_TestGameMode::SpawnForward()
{
	if (ForwardTiles.Num()>0)
	{
		int randomInt =(ForwardTiles.Num()==1) ? 0 : FMath::RandRange(0, ForwardTiles.Num()-1);//Get (random) integer for tile choosing
		SpawnTileActorAtAttachLocation(ForwardTiles[randomInt]);
		tileForwardCount++;
	}
	
}

void AEndlessRunner_TestGameMode::SpawnTurn()
{
	if (TurnedTiles.Num() > 0)
	{
		int randomInt = (TurnedTiles.Num() == 1) ? 0 : FMath::RandRange(0, TurnedTiles.Num() - 1);
		SpawnTileActorAtAttachLocation(TurnedTiles[randomInt]);
		tileForwardCount=0;
	}
	else SpawnForward(); //Safeguard for lack of turning tiles
}

ABaseTile* AEndlessRunner_TestGameMode::SpawnTileActorAtAttachLocation(TSubclassOf<class ABaseTile> TileActor)
{
	UWorld* const World = this->GetWorld();
	ABaseTile* tile = nullptr;
	if (World){
		tile = World->SpawnActor<ABaseTile>(TileActor, nextAttachPoint.GetLocation(), nextAttachPoint.GetRotation().Rotator());
		nextAttachPoint = tile->getEndPointTransform();
	}
	return tile;
}

void AEndlessRunner_TestGameMode::BeginPlay()
{
	FTransform playerStartTransform = FindPlayerStart(GetWorld()->GetFirstPlayerController(), FString("Start"))->GetTransform();
	playerStartTransform.SetLocation(playerStartTransform.GetLocation() - FVector(100, 0, 50));
	nextAttachPoint = playerStartTransform;

	for (int i = 0; i < 6; i++)
	{
		SpawnTile();
	}
}
