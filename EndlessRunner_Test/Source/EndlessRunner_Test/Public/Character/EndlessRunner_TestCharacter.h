// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "EndlessRunner_TestCharacter.generated.h"

UENUM()
enum class RunnerLane : uint8
{
	LeftLane,
	MiddleLeftLane,
	MiddleLane,
	MiddleRightLane,
	RightLane
};

UCLASS(config=Game)
class AEndlessRunner_TestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


	
public:
	AEndlessRunner_TestCharacter(const FObjectInitializer& ObjectInitializer);

	//determines whether the player is turning
	bool isTurning;

	//stores the lane the player currently runs in
	RunnerLane currentLane;

	/**variables used for eased transition from currentlane to desiredlane*/
	bool isSwitching;
	FVector desiredLocation;
	

	/**Distance the player has to cross for the next lane*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Runner)
	float LaneStepDistance;

	/**speed at which the player switches lanes*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Runner)
		float LaneSwitchSpeed;

	/**Indicates whether the player is allowed to switch lane's*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Runner)
	bool canSwitchLane;

	/**The lane the player wants to switch to*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Runner)
		RunnerLane desiredLane;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Indication whether the player can turn*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Runner)
	bool CanTurn;

	/**Base Speed with which the player runs the endless course*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Runner)
	float BaseSpeed;

	/**Desired rotation for a turn*/
	UPROPERTY(BlueprintReadOnly, Category = Turn)
		FRotator DesiredRotation;

	bool correcting;
	bool needsCorrecting;

private:
	UPROPERTY()
	int32 Score;

protected:

	void Tick(float DeltaSeconds) override;

	/** Called for forwards/backward movement */
	void MoveForward(float Value);

	/** Called for move right input */
	void MoveRight();

	/** Called for move left input*/
	void MoveLeft();

	/**Checks player rotation against desired rotation. If unequal, lerps to desired rotation
	@param world delta time
	*/
	void TurnCorner(float DeltaTime);


	/**Checks player's current lane. If unequal to desired lane, moves player to desired lane
	@param DeltaTime World delta time
	*/
	void SwitchLane(float DeltaTime);

	void MoveToLane(float DeltaTime);


public:

	UFUNCTION()
		virtual void BeginDestroy() override;

	UFUNCTION()
		void LocationCorrection(FVector MiddleLaneLocation);

	/**Called to increase player score by value
	@param Value the value by which the score is increased
	*/
	UFUNCTION(BlueprintCallable, Category = Score)
		void IncreaseScore(int32 Value);
private:
	
	/**
	Creates Vector based on direction and desired lane
	Reduces the vector to only contain the necessary axis data
	@param axisVector a vector with only the necessary axis //Example: X-axis = (1,0,0)
	@return Vector location of desired lane
	*/
	FVector getLaneAxisLocation(FVector axisVector);

	


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	

};

