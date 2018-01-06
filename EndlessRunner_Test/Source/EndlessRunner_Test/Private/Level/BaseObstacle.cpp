// Fill out your copyright notice in the Description page of Project Settings.

#include "EndlessRunner_Test.h"
#include "BaseObstacle.h"
#include "EndlessRunner_TestCharacter.h"


// Sets default values
ABaseObstacle::ABaseObstacle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Obstacle Mesh"));
	ObstacleMesh->AttachTo(RootComponent);
	ObstacleMesh->SetCollisionResponseToAllChannels(ECR_Block);//Make Mesh overlap all actors
	ObstacleMesh->OnComponentHit.AddDynamic(this, &ABaseObstacle::OnHit);
}

// Called when the game starts or when spawned
void ABaseObstacle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseObstacle::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ABaseObstacle::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AEndlessRunner_TestCharacter* Character = Cast<AEndlessRunner_TestCharacter>(OtherActor);//Cast overlapping actor to TestCharacter
	if (Character->IsValidLowLevel())//check if actor is valid
	{
		onCharacterCollide(Character, OtherComp);

	}
}

void ABaseObstacle::onCharacterCollide_Implementation(class AEndlessRunner_TestCharacter* character, class UPrimitiveComponent* OtherComp)
{
	
}

void ABaseObstacle::HitCharacter_Implementation(class AEndlessRunner_TestCharacter* character, class UPrimitiveComponent* OtherComp)
{
	character->DisableInput(GetWorld()->GetFirstPlayerController());
	character->GetRootComponent()->SetVisibility(false, true);
	FTimerHandle RestartGame;
	GetWorldTimerManager().SetTimer(RestartGame, this, &ABaseObstacle::RestartGame, 2.0f, false, -1.0f);
}

void ABaseObstacle::RestartGame()
{
	GetWorld()->Exec(GetWorld(), TEXT("RestartLevel"));
}

