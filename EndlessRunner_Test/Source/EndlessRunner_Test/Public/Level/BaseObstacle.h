// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseObstacle.generated.h"

UCLASS(ABSTRACT)
class ENDLESSRUNNER_TEST_API ABaseObstacle : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseObastacle, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseObastacle, meta = (AllowPrivateAccess = "true", DisplayName = "Obstacle"))
	class UStaticMeshComponent* ObstacleMesh;

	
public:	
	// Sets default values for this actor's properties
	ABaseObstacle(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** called when something overlaps the CoinMesh component */
	UFUNCTION()
		void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//Blueprint overridable event with native implementation called when character collides with mesh
	UFUNCTION(BlueprintNativeEvent)
		void onCharacterCollide(class AEndlessRunner_TestCharacter* character, class UPrimitiveComponent* OtherComp);
	virtual void onCharacterCollide_Implementation(class AEndlessRunner_TestCharacter* character, class UPrimitiveComponent* OtherComp);

	//Blueprint overridable event with native implementation  for what happens with the character if he hits an obstacle
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = BaseObastacle)
		void HitCharacter(class AEndlessRunner_TestCharacter* character, class UPrimitiveComponent* OtherComp);
	virtual void HitCharacter_Implementation(class AEndlessRunner_TestCharacter* character, class UPrimitiveComponent* OtherComp);
private:
	void RestartGame();
	
};
