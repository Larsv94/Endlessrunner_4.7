// Fill out your copyright notice in the Description page of Project Settings.

#include "EndlessRunner_Test.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EndlessRunner_TestCharacter.h"
#include "EndlessRunner_TestGameMode.h"
#include "BaseTile.h"

#define print(position,text) if (GEngine) GEngine->AddOnScreenDebugMessage(position, 1.5, FColor::Red,text)//easy debug macro


// Sets default values
ABaseTile::ABaseTile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TileScene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = TileScene;

	EndPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Endpoint"));
	EndPoint->AttachTo(RootComponent);

	TileFloor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	TileFloor->AttachTo(RootComponent);

	EntryBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Entry Box"));
	EntryBox->AttachTo(RootComponent);
	EntryBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseTile::OnOverlapBegin_Entry);

	ExitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Exit Box"));
	ExitBox->AttachTo(RootComponent);
	ExitBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseTile::OnOverlapBegin_Exit),

	//Set class defaults
	EndPointDirection = ConnectionDirection::D_Forward;//attach point defaults to forward
	bSpawnCoins, bSpawnObstacles = false;


}

//Called whenever a property changes, the actor is spawned or the actor is moved
void ABaseTile::OnConstruction(const FTransform& Transform)
{
	
	

	FVector Origin;//Used to store origin from floor
	FVector BoxExtent;//Used to store BoxExtent from floor
	float SphereRadius;//Used to store Sphere Radius from floor
	UKismetSystemLibrary::GetComponentBounds(TileFloor, Origin, BoxExtent, SphereRadius);//Gets and stores Origin, BoxExtent and SphereRadius from TileFloor

	TileFloor->SetRelativeLocation(FVector(BoxExtent.X,0,0));//set tile floor location beyond actor pivot point

	EndPoint->SetRelativeTransform(getConnectionAttachPoint(EndPointDirection,BoxExtent));//Relocate Endpoint to desired relative transform based on Endpoint Direction

	//Position EntryBox relative from scene center and at the begin of the floor
	FTransform tempTransform = EntryBox->GetRelativeTransform();
	tempTransform.SetScale3D(FVector(0.1, BoxExtent.Y / 15 / 2, 10));
	tempTransform.SetLocation( FVector(20, 0, 0));
	EntryBox->SetRelativeTransform(tempTransform);

	//Position EntryBox relative from scene center and at the end of the floor
	tempTransform = ExitBox->GetRelativeTransform();
	tempTransform.SetScale3D(FVector(0.1, BoxExtent.Y/15 / 2, 10));
	tempTransform.SetLocation(EndPoint->GetRelativeTransform().GetLocation()-FVector(20,0,0));
	tempTransform.SetRotation(EndPoint->GetRelativeTransform().GetRotation());//Set rotation in case the tile is a turn
	ExitBox->SetRelativeTransform(tempTransform);

	

	//Spawn obstacles and coins
	if (bSpawnCoins&&CoinAmount > 0 && CoinsToSpawn.Num() > 0)
	{ 
		CoinsToSpawn.Sort();//Sort array based on weight
		SpawnLevelObjects(FName("coins"), CoinSpawnPoints, CoinAmount, CoinsToSpawn, SpawnedCoins);
	}
	if (bSpawnObstacles&&ObstacleAmount > 0 && ObstaclesToSpawn.Num() > 0)
	{
		ObstaclesToSpawn.Sort();//Sort array based on weight
		SpawnLevelObjects(FName("obstacles"), ObstacleSpawnPoints, ObstacleAmount, ObstaclesToSpawn, SpawnedObstacles);
	}
	
}

UClass* ABaseTile::GetRandomClassByWeight(TArray<FWeightCoinStruct> & coinArray)
{
	TArray<int32> weights;
	//Store all weights
	for (auto i = 0; i < coinArray.Num(); i++)
	{
		weights.Add(coinArray[i].weight);
	}
	return coinArray[GetIndexByWeight(weights)].CoinType;
}

UClass* ABaseTile::GetRandomClassByWeight(TArray<FWeightObstacleStruct> & obstacleArray)
{
	TArray<int32> weights;
	//Store all weights
	for (auto i = 0; i < obstacleArray.Num(); i++)
	{
		weights.Add(obstacleArray[i].weight);
	}
	return obstacleArray[GetIndexByWeight(weights)].ObstacleType;
}

int32 ABaseTile::GetIndexByWeight(TArray<int32> weights)
{
		
	TArray<int32> totals;
	int32 running_total = 0;

	for (int32 var : weights)
	{
		running_total += var;
		totals.Add(running_total);
	}

	int32 rnd = FMath::RandRange(0,running_total);
	for (auto i = 0; i < totals.Num(); i++)
	{
		if (rnd < totals[i]) {
			return i;
		}
	}
	return NULL;//should probably never come here
}

void ABaseTile::findSpawnPointComponentsByTag(FName tag, TArray<USceneComponent* > & storeArray, int32 & amount){
	storeArray.Empty();
	TArray<UActorComponent*> temporaryArray = this->GetComponentsByTag(USceneComponent::StaticClass(),tag);
	TArray<USceneComponent*> SceneComponentArrayTemp;

	//Convert UActorComponents to USceneComponents and store in TArray
	for (int32 i = 0; i < temporaryArray.Num(); i++)
	{
		USceneComponent* var = Cast<USceneComponent>(temporaryArray[i]);
		SceneComponentArrayTemp.Add(var);
	}

	//amount can't be higher than SceneComponentArrayTemp length
	if (SceneComponentArrayTemp.Num()<amount)
	{
		amount = SceneComponentArrayTemp.Num();
	}

	while (storeArray.Num() < amount)
	{
		int32 debugint = SceneComponentArrayTemp.Num();
		bool debug = (SceneComponentArrayTemp.Num() <= 1);
		int32 rndInt = debug ?  0 : FMath::RandRange(0, debugint - 1) ;
		USceneComponent*  tempComponent = SceneComponentArrayTemp[rndInt];
		if (!storeArray.Contains(tempComponent))
		{
			
			storeArray.Add(tempComponent);
		}
	}
	
}

UChildActorComponent* ABaseTile::SpawnChild(UClass* CompClass, FVector Location, FRotator Rotation, FName ComponentName)
{
	UChildActorComponent* NewComponent = ConstructObject<UChildActorComponent>(UChildActorComponent::StaticClass(), this, ComponentName);//Create new component
	NewComponent->ChildActorClass = CompClass;
	NewComponent->CreateChildActor();
	if (!NewComponent)
	{
		return nullptr;//if creation failed return null pointer
	}
	else{
		NewComponent->RegisterComponent();//Register component
		NewComponent->SetWorldLocation(Location);
		NewComponent->SetWorldRotation(Rotation);
		NewComponent->AttachTo(RootComponent, FName(""), EAttachLocation::KeepWorldPosition);
		return NewComponent;
	}
}

void ABaseTile::SpawnLevelObjects(FName tag, TArray<USceneComponent* > & storeArray, int32 & amount, TArray<FWeightCoinStruct> weightedArray, TArray<UChildActorComponent*> & ComponentArray)
{
	for (UChildActorComponent* child : ComponentArray)
	{
		if (child)
		{

			//child->DestroyChildActor();
			child->DestroyComponent();

		}
	}
	ComponentArray.Empty();

	ABaseTile::findSpawnPointComponentsByTag(tag, storeArray, amount);
	if (storeArray.Num() > 0)
	{
		for (int32 i = 0; i < storeArray.Num(); i++)
		{
			UClass* CompClass = GetRandomClassByWeight(weightedArray);
			USceneComponent*  sceneComp = storeArray[i];
			FName newComponentName = FName(*(tag.ToString().Append(FString::FromInt(i)))); //Construct name from Tag and array position
			UChildActorComponent* newComponent = SpawnChild(CompClass, sceneComp->GetComponentLocation(), sceneComp->GetComponentRotation(), newComponentName);
			if (newComponent)
			{
				ComponentArray.Add(newComponent);
			}

		}

	}

}

void ABaseTile::SpawnLevelObjects(FName tag, TArray<USceneComponent* > & storeArray, int32 & amount,TArray<FWeightObstacleStruct> weightedArray, TArray<UChildActorComponent*> & ComponentArray)
{
	for (UChildActorComponent* child : ComponentArray)
	{
		if (child)
		{

			//child->DestroyChildActor();
			child->DestroyComponent();

		}
	}
	ComponentArray.Empty();

	ABaseTile::findSpawnPointComponentsByTag(tag, storeArray, amount);
	if (storeArray.Num() > 0)
	{
		for (int32 i = 0; i < storeArray.Num(); i++)
		{
			UClass* CompClass = GetRandomClassByWeight(weightedArray);
			USceneComponent*  sceneComp = storeArray[i];
			FName newComponentName = FName(*(tag.ToString().Append(FString::FromInt(i)))); //Construct name from Tag and array position
			UChildActorComponent* newComponent = SpawnChild(CompClass, sceneComp->GetComponentLocation(), sceneComp->GetComponentRotation(), newComponentName);
			if (newComponent)
			{
				ComponentArray.Add(newComponent);
			}

		}

	}

}


void ABaseTile::OnOverlapBegin_Entry(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	FVector Origin;//Used to store origin from floor
	FVector BoxExtent;//Used to store BoxExtent from floor
	float SphereRadius;//Used to store Sphere Radius from floor
	UKismetSystemLibrary::GetComponentBounds(TileFloor, Origin, BoxExtent, SphereRadius);//Gets and stores Origin, BoxExtent and SphereRadius from TileFloor

	AEndlessRunner_TestCharacter* character = Cast<AEndlessRunner_TestCharacter>(OtherActor);

	if (character->IsValidLowLevel())
	{
		character->LocationCorrection(Origin);//Initiate position correction 
	}

}

void ABaseTile::OnOverlapBegin_Exit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AEndlessRunner_TestCharacter* character = Cast<AEndlessRunner_TestCharacter>(OtherActor);

	if (character->IsValidLowLevel())
	{
		AEndlessRunner_TestGameMode* gamemode = (AEndlessRunner_TestGameMode*)GetWorld()->GetAuthGameMode();
		if (gamemode->IsValidLowLevel())
		{
			gamemode->SpawnTile();//Spawn new tile
			FTimerHandle DestroyActor;
			GetWorldTimerManager().SetTimer(DestroyActor, this, &ABaseTile::DestroyThisTile, 2.0f, false, -1.0f);//Set world timer to destroy this tile
		}
	}
}

//Called when tile is destroyed when character leaves the tile
void ABaseTile::DestroyThisTile()
{
	this->Destroy();
}




// Called when the game starts or when spawned
void ABaseTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseTile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

FTransform ABaseTile::getEndPointTransform()
{
	return EndPoint->GetComponentTransform();
}


FTransform ABaseTile::getConnectionAttachPoint(ConnectionDirection direction, FVector FloorExtend)
{
	FRotator rotation;
	FVector location;
	switch (direction)
	{
	case ConnectionDirection::D_Forward:
		rotation = FRotator(0, 0, 0);
		location = FVector(FloorExtend.X * 2, 0, EndPoint->GetRelativeTransform().GetLocation().Z);//Use Y location from component so user can choose attach height
		break;
	case ConnectionDirection::D_Right:
		rotation = FRotator(0, 90, 0);
		location = FVector(FloorExtend.X, FloorExtend.Y , EndPoint->GetRelativeTransform().GetLocation().Z);//Use Z location from component so user can choose attach height
		break;
	case ConnectionDirection::D_Left:
		rotation = FRotator(0, -90, 0);
		location = FVector(FloorExtend.X,  FloorExtend.Y * -1, EndPoint->GetRelativeTransform().GetLocation().Z);//Use Z location from component so user can choose attach height
		break;
	default:
		//Never reached
		rotation = FRotator::ZeroRotator;
		location = FVector::ZeroVector;
		break;
	}

	FTransform tempTransf = FTransform::FTransform();//Create empty transform
	tempTransf.SetLocation(location);
	tempTransf.SetRotation(rotation.Quaternion());
	return tempTransf;
}



