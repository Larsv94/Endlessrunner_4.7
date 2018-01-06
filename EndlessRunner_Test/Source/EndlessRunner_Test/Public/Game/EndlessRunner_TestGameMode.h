// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "EndlessRunner_TestGameMode.generated.h"

UCLASS(minimalapi)
class AEndlessRunner_TestGameMode : public AGameMode
{
	GENERATED_BODY()
protected:

	int tileForwardCount;
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FTransform nextAttachPoint;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<class ABaseTile>> ForwardTiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<class ABaseTile>> TurnedTiles;

	

protected:


	/**
	Spawn a type of BaseTile at nextAttachPoint location
	@param TileActor the subclass actor which has to be spawned
	@return the spawned BaseTile Actor
	*/
	UFUNCTION(Category = Default)
	ABaseTile* SpawnTileActorAtAttachLocation(TSubclassOf<class ABaseTile> TileActor);

public:
	AEndlessRunner_TestGameMode(const FObjectInitializer& ObjectInitializer);



	UFUNCTION(BlueprintCallable, Category = Default)
		void SpawnTile();
	UFUNCTION(BlueprintCallable, Category = Default)
		void SpawnForward();
	UFUNCTION(BlueprintCallable, Category = Default)
		void SpawnTurn();


	virtual void BeginPlay() override;

};



