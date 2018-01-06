// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "EndlessRunner_Test.h"
#include "Kismet/KismetMathLibrary.h"
#include "EndlessRunner_TestCharacter.h"

#define west FVector(0,-1,0)
#define north FVector(1,0,0)
#define south FVector(1,0,0)
#define east FVector(0,1,0)


#define print(position,text) if (GEngine) GEngine->AddOnScreenDebugMessage(position, 1.5, FColor::Red,text)//easy debug macro

//////////////////////////////////////////////////////////////////////////
// AEndlessRunner_TestCharacter

AEndlessRunner_TestCharacter::AEndlessRunner_TestCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->IsActorTickEnabled();

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// set our default runner values
	BaseSpeed = 1.0f;
	CanTurn = false;
	currentLane = RunnerLane::MiddleLane;
	desiredLane = RunnerLane::MiddleLane;
	LaneStepDistance = 500.0f;
	LaneSwitchSpeed = 5.0f;
	canSwitchLane = true;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->SocketOffset = FVector(0, 0, 100);//Set Camera offset above head of character
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller


	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

//////////////////////////////////////////////////////////////////////////
// Input

void AEndlessRunner_TestCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("TurnLeft", IE_Pressed, this, &AEndlessRunner_TestCharacter::MoveLeft);

	InputComponent->BindAction("TurnRight", IE_Pressed, this, &AEndlessRunner_TestCharacter::MoveRight);



}


void AEndlessRunner_TestCharacter::BeginDestroy()
{
	//GetWorld()->Exec(GetWorld(), TEXT("Restart"));
	Super::BeginDestroy();
}

void AEndlessRunner_TestCharacter::Tick(float DeltaSeconds) {
	//Move character with base speed every tick
	MoveForward(BaseSpeed);

	//Turns corner if player is allowed. Checks every tick.
	TurnCorner(DeltaSeconds);
	//Switches lanes if player is allowed. Checks every tick
	if (correcting)
	{
		//If position needs correcting move to correct position directly, else use default switchlane
		MoveToLane(DeltaSeconds);
	}
	else SwitchLane(DeltaSeconds);
	}

void AEndlessRunner_TestCharacter::MoveForward(float Value)
{
	if ((Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		
	}
}

void AEndlessRunner_TestCharacter::MoveRight()
{
	
	if ( (Controller != NULL)&&CanTurn)
	{
		needsCorrecting = true;
		DesiredRotation +=FRotator(0,90,0);//Increase desired rotation yaw with 90 to turn to the right
		isTurning = true;
		CanTurn = false;
	}
	else if ((uint8)currentLane<4&&canSwitchLane)
	{
		desiredLane = RunnerLane((uint8)currentLane + 1);//set desired lane to the right of the current lane
		
	}

}

void AEndlessRunner_TestCharacter::MoveLeft(){

	if ((Controller != NULL)&&CanTurn)
	{
		needsCorrecting = true;
		DesiredRotation += FRotator(0, -90, 0);//Decrease desired rotation yaw with 90 to turn to the left
		CanTurn = false;
		isTurning = true;
	}
	else if((uint8)currentLane>0&&canSwitchLane)
	{
		desiredLane = RunnerLane((uint8)currentLane - 1);//set desired lane to the left of the current lane
	}
}

void AEndlessRunner_TestCharacter::TurnCorner(float DeltaTime)
{

	float rotationDifference = (DesiredRotation.Clamp() - ACharacter::GetActorRotation()).Clamp().Yaw; //get the difference between Desired Rotation and Actor Rotation on Yaw Axis
	float rotationAbsNearlyZero = FMath::Abs((rotationDifference > 180) ? rotationDifference - 360 : rotationDifference);//Character never turns 180 degrees at once so subtract 360 when higher than 180 to keep value around zero

	if (isTurning)
	{
		canSwitchLane = false;
		if ((double)rotationAbsNearlyZero<(double)0.2000000000000000000000000000000000001||FMath::IsNearlyZero(rotationAbsNearlyZero))//Error margin check
		{
			Controller->SetControlRotation(DesiredRotation.Clamp());//Sets ControlRotation to DesiredRotation to prevent errors with the Interpolation
			isTurning = false;
			canSwitchLane = true;
		}
		else 
		{
			Controller->SetControlRotation(FMath::RInterpTo(ACharacter::GetActorRotation(), DesiredRotation.Clamp(), DeltaTime,10.0f));
		
		
		}
	}
	
}

void AEndlessRunner_TestCharacter::SwitchLane(float DeltaTime)
{
	if (currentLane!=desiredLane){
		FVector multiplyVector = (AEndlessRunner_TestCharacter::GetActorForwardVector().GetAbs().X == 1) ? FVector(0, 1, 0) : FVector(1, 0, 0);//Determine on which axis the player has to check movement
		if (!isSwitching)
		{
			canSwitchLane = false;
			desiredLocation = getLaneAxisLocation(multiplyVector);
			isSwitching = true;
		}
		else
		{
			MoveToLane(DeltaTime);
		}

	}
	
}

void AEndlessRunner_TestCharacter::MoveToLane(float DeltaTime)
{
	FVector multiplyVector = (AEndlessRunner_TestCharacter::GetActorForwardVector().GetAbs().X == 1) ? FVector(0, 1, 0) : FVector(1, 0, 0);//Determine on which axis the player has to check movement

	FVector CurrentPosition = AEndlessRunner_TestCharacter::GetActorLocation();//Get current world space location of character

	float vectorDistance = FVector::Dist(CurrentPosition*multiplyVector, desiredLocation);

	if (vectorDistance<0.1f || FMath::IsNearlyZero(vectorDistance))
	{
		if (AEndlessRunner_TestCharacter::GetActorForwardVector().GetAbs().X == 1)
		{
			AEndlessRunner_TestCharacter::SetActorLocation(FVector(CurrentPosition.X, desiredLocation.Y, CurrentPosition.Z));
		}
		else{
			AEndlessRunner_TestCharacter::SetActorLocation(FVector(desiredLocation.X, CurrentPosition.Y, CurrentPosition.Z));
		}
		currentLane = desiredLane;
		isSwitching = false;
		canSwitchLane = true;
		correcting = false;
	}
	if ((CurrentPosition * multiplyVector) != desiredLocation)
	{
		float vectorDistance = FVector::Dist(CurrentPosition*multiplyVector, desiredLocation);//get distance between player and desiredlocation
		float TransitionTime = LaneSwitchSpeed / (vectorDistance / LaneStepDistance);//Linear Interpolate time based on distance
		FVector moveToVector = FMath::VInterpTo(CurrentPosition*multiplyVector, desiredLocation, DeltaTime, FMath::Abs<float>(TransitionTime));//Store vector from linear vector interpolation

		if (AEndlessRunner_TestCharacter::GetActorForwardVector().GetAbs().X == 1)
		{
			AEndlessRunner_TestCharacter::SetActorLocation(FVector(CurrentPosition.X, moveToVector.Y, CurrentPosition.Z));
		}
		else{
			AEndlessRunner_TestCharacter::SetActorLocation(FVector(moveToVector.X, CurrentPosition.Y, CurrentPosition.Z));
		}


	}
}

FVector AEndlessRunner_TestCharacter::getLaneAxisLocation(FVector axisVector)
{
	FVector forwardVector = GetActorForwardVector();
	float LaneAxisDistance = (currentLane > desiredLane) ? LaneStepDistance : LaneStepDistance*-1;

	if (forwardVector == south || forwardVector == east || forwardVector.Equals(FVector(0, -1, 0))){//for a strange reason forwardVector == west doesn't work. So uses forwardVector.Equals(FVector(0, -1, 0) instead
		LaneAxisDistance *= (-1);
	}
	return (GetActorLocation()*axisVector)+axisVector * LaneAxisDistance;//return current location increased with laneAxisDistance
	
}

void AEndlessRunner_TestCharacter::LocationCorrection(FVector MiddleLaneLocation)
{
	if (needsCorrecting)
	{
	
	FVector multiplyVector = (AEndlessRunner_TestCharacter::GetActorForwardVector().GetAbs().X == 1) ? FVector(0, 1, 0) : FVector(1, 0, 0);//Determine on which axis the player has to check movement
	FVector newLocation = MiddleLaneLocation*multiplyVector;
	int axisCorrection = (GetActorForwardVector() == north) ? 1 : -1; //Used to correct coordinate system based on rotation
	switch (currentLane)//create newlocation based on MiddleLaneLocation
	{
	case RunnerLane::LeftLane:
		newLocation -= multiplyVector*(LaneStepDistance * 2 *axisCorrection);
		break;
	case RunnerLane::MiddleLeftLane:
		newLocation -= multiplyVector*(LaneStepDistance  *axisCorrection);
		break;
	case RunnerLane::MiddleRightLane:
		newLocation += multiplyVector*(LaneStepDistance  *axisCorrection);
		break;
	case RunnerLane::RightLane:
		newLocation += multiplyVector*(LaneStepDistance * 2 * axisCorrection);
		break;
	default:
		break;
	}
	desiredLocation = newLocation;
	correcting = true;
	canSwitchLane = false;
	needsCorrecting = false;
	}
}

void AEndlessRunner_TestCharacter::IncreaseScore(int32 Value)
{
	if (Value>0)//Don't allow negative increments
	{
		Score += Value;
	}
	
}

