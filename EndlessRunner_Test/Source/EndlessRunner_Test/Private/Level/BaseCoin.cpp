// Fill out your copyright notice in the Description page of Project Settings.

#include "EndlessRunner_Test.h"
#include "BaseCoin.h"
#include "EndlessRunner_TestCharacter.h"
#define print(position,text) if (GEngine) GEngine->AddOnScreenDebugMessage(position, 1.5, FColor::Red,text)//easy debug macro

// Sets default values
ABaseCoin::ABaseCoin(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Coin Mesh"));
	CoinMesh->AttachTo(RootComponent);
	CoinMesh->SetCollisionResponseToAllChannels(ECR_Overlap);//Make Mesh overlap all actors
	CoinMesh->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoin::OnOverlapBegin);



}

// Called when the game starts or when spawned
void ABaseCoin::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCoin::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ABaseCoin::onPickUp_Implementation(class AEndlessRunner_TestCharacter* character, class UPrimitiveComponent* OtherComp)
{
	character->IncreaseScore(CoinValue);
	DestroyCoin();
}

void ABaseCoin::DestroyCoin(){
	Destroy();
}

void ABaseCoin::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AEndlessRunner_TestCharacter* Character = Cast<AEndlessRunner_TestCharacter>(OtherActor);//Cast overlapping actor to TestCharacter
	if (Character)//check if actor is valid
	{
		onPickUp(Character, OtherComp);
	}
}

