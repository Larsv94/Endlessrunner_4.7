// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseCoin.generated.h"

UCLASS(ABSTRACT, meta = (BlueprintSpawnableComponent))
class ENDLESSRUNNER_TEST_API ABaseCoin : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseCoin, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseCoin, meta = (AllowPrivateAccess = "true", DisplayName = "Coin"))
	class UStaticMeshComponent* CoinMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseCoin, meta = (AllowPrivateAccess = "true", DisplayName = "Coin Value"))
	int32 CoinValue;


public:	
	// Sets default values for this actor's properties
	ABaseCoin(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** called when something overlaps the CoinMesh component */
	UFUNCTION()
		void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//Blueprint overridable event called when character overlaps with mesh
	UFUNCTION(BlueprintNativeEvent)
		void onPickUp(class AEndlessRunner_TestCharacter* character, class UPrimitiveComponent* OtherComp);
	virtual void onPickUp_Implementation(class AEndlessRunner_TestCharacter* character, class UPrimitiveComponent* OtherComp);
	
private:
	void DestroyCoin();
};
