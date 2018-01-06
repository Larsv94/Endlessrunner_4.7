// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Level/BaseCoin.h"
#include "Level/BaseObstacle.h"
#include "BaseTile.generated.h"

USTRUCT(BlueprintType)
struct FWeightCoinStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "BaseTile")
	TSubclassOf<ABaseCoin> CoinType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BaseTile")
		int32 weight;

	//Operator overloading for <
	friend bool operator<(const FWeightCoinStruct  thisstruct, const FWeightCoinStruct  otherStruct);
	FORCEINLINE friend bool operator<(const FWeightCoinStruct  thisstruct, const FWeightCoinStruct  otherStruct)
	{
	
		return (thisstruct.weight < otherStruct.weight);
	}


};

USTRUCT(BlueprintType)
struct FWeightObstacleStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BaseTile")
	TSubclassOf<ABaseObstacle> ObstacleType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BaseTile")
		int32 weight;

	//Operator overloading for <
	friend bool operator<(const FWeightObstacleStruct  thisstruct, const FWeightObstacleStruct  otherStruct);
	FORCEINLINE friend bool operator<(const FWeightObstacleStruct  thisstruct, const FWeightObstacleStruct  otherStruct)
	{

		return (thisstruct.weight < otherStruct.weight);
	}


};

UENUM()
enum class ConnectionDirection : uint8
{
	D_Forward UMETA(DisplayName = "Forward"),
	D_Right UMETA(DisplayName = "Right"),
	D_Left UMETA(DisplayName = "Left")
};

UCLASS(ABSTRACT)
class ENDLESSRUNNER_TEST_API ABaseTile : public AActor
{
	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseTile, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* TileScene;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseTile, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* EndPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseTile, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* TileFloor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseTile, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* EntryBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseTile, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ExitBox;



public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BaseTile, meta = (AllowPrivateAcces = "true"))
		ConnectionDirection EndPointDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseTile , meta = (DisplayName = "Spawn Coins"))
		bool bSpawnCoins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseTile, meta = (DisplayName = "Spawn Obstacles"))
		bool bSpawnObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseTile, meta = (DisplayName = "Coin amount"))
		int32 CoinAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseTile, meta = (DisplayName = "Obstacle amount"))
		int32 ObstacleAmount;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BaseTile, meta = (DisplayName = "Coins To Spawn"))
	TArray<FWeightCoinStruct> CoinsToSpawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BaseTile, meta = (DisplayName = "Obstacles To Spawn"))
		TArray<FWeightObstacleStruct> ObstaclesToSpawn;

private:

	//Can't be UPROPERTY for a weird reason(probably fixed in 4.8)
		TArray<USceneComponent*> CoinSpawnPoints;
		TArray<USceneComponent*> ObstacleSpawnPoints;

	UPROPERTY()
		TArray<UChildActorComponent*> SpawnedObstacles;

	UPROPERTY()
		TArray<UChildActorComponent*> SpawnedCoins;


protected:

	/** called when something enters the EntryBox component */
	UFUNCTION()
		void OnOverlapBegin_Entry(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** called when something enters the ExitBox component */
	UFUNCTION()
		void OnOverlapBegin_Exit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	//Called when this tile is destroyed after character leaves tile
	void DestroyThisTile();
public:
	
	// Sets default values for this actor's properties
	ABaseTile(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//Called on Spawn, change or reposition of actor
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category = "BaseTile")
	FTransform getEndPointTransform();
	

private:


	UClass* GetRandomClassByWeight(TArray<FWeightObstacleStruct> & obstacleArray);
	UClass* GetRandomClassByWeight(TArray<FWeightCoinStruct> & coinArray);

	/**
	Get random index based on weight
	@param weights (sorted)array filled with weights
	*/
	int32 GetIndexByWeight(TArray<int32> weights);

	/**
	*/
	void findSpawnPointComponentsByTag(FName tag, TArray<USceneComponent* > & storeArray, int32 & amount);


	UChildActorComponent* SpawnChild(UClass* CompClass, FVector Location, FRotator Rotation, FName ComponentName);

	void SpawnLevelObjects(FName tag, TArray<USceneComponent* > & storeArray, int32 & amount, TArray<FWeightCoinStruct> weightedArray, TArray<UChildActorComponent*> & ComponentArray);
	void SpawnLevelObjects(FName tag, TArray<USceneComponent* > & storeArray, int32 & amount, TArray<FWeightObstacleStruct> weightedArray, TArray<UChildActorComponent*> & ComponentArray);
	/**
	Calculates attach point for Endpoint based on the desired Direction
	@param Direction the desired ConnectionDirection
	@param FloorExtend the unit size of the Floor mesh
	@return FTransform returns the relative position and rotation of the attach point in FTransform
	*/
	FTransform getConnectionAttachPoint(ConnectionDirection Direction, FVector FloorExtend);

	
	
};
