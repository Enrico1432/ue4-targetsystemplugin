// Copyright 2018-2019 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerController.h"
#include "TargetSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponentOnTargetLockedOnOff, AActor*, TargetActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FComponentSetRotation, AActor*, TargetActor, FRotator, ControlRotation);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARGETSYSTEM_API UTargetSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTargetSystemComponent();

	// The minimum distance to enable target locked on.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	float MinimumDistanceToEnable;

	// The AActor Subclass to search for targetable Actors.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
    TSubclassOf<AActor> TargetableActors;

	// Whether or not the character rotation should be controlled when Target is locked on.
	//
	// If true, it'll set the value of bUseControllerRotationYaw and bOrientationToMovement variables on Target locked on / off.
	//
	// Set it to true if you want the character to rotate around the locked on target to enable you to setup strafe animations.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
    bool bShouldControlRotation;

	// The amount of time to break line of sight when actor gets behind an Object.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
    float BreakLineOfSightDelay;

	// Lower this value is, easier it will be to switch new target on right or left.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
    float StartRotatingThreshold;

	// Whether or not the Target LockOn Widget indicator should be drawn and attached automatically.
	//
	// When set to false, this allow you to manually draw the widget for further control on where you'd like it to appear.
	//
	// OnTargetLockedOn and OnTargetLockedOff events can be used for this.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
	bool bShouldDrawLockedOnWidget;

	// The Widget Class to use when locked on Target. If not defined, will fallback to a Text-rendered
	// widget with a single O character.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
    TSubclassOf<UUserWidget> LockedOnWidgetClass;

	// The Widget Draw Size for the Widget class to use when locked on Target.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
    float LockedOnWidgetDrawSize;

	// The Socket name to attach the LockedOn Widget.
	//
	// You should use this to configure the Bone or Socket name the widget should be attached to, and allow
	// the widget to move with target character's animation (Ex: spine_03)
	//
	// Set it to None to attach the Widget Component to the Root Component instead of the Mesh.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
    FName LockedOnWidgetParentSocket;

	// The Relative Location to apply on Target LockedOn Widget when attached to a target.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
    FVector LockedOnWidgetRelativeLocation;

	// Setting this to true will tell the Target System to adjust the Pitch Offset (the Y axis) when locked on,
	// depending on the distance to the target actor.
	//
	// It will ensure that the Camera will be moved up vertically the closer this Actor gets to its target.
	//
	// Formula:
	//
	//   (DistanceToTarget * PitchDistanceCoefficient + PitchDistanceOffset) * -1.0f
	//
	// Then Clamped by PitchMin / PitchMax
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Pitch Offset")
	bool bAdjustPitchBasedOnDistanceToTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Pitch Offset")
    float PitchDistanceCoefficient = -0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Pitch Offset")
    float PitchDistanceOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Pitch Offset")
	float PitchMin = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Pitch Offset")
	float PitchMax = -20.0f;

	// Function to call to target a new actor.
	UFUNCTION(BlueprintCallable, Category = "Target System")
	void TargetActor();

	// Function to call to manually untarget.
	UFUNCTION(BlueprintCallable, Category = "Target System")
	void TargetLockOff();

	/**
	* Function to call to switch with X-Axis mouse / controller stick movement.
	*
	* @param AxisValue Pass in the float value of your Input Axis
	* @param Delta This value and AxisValue are multiplied before being checked against StartRotatingThreshold
	*/
	UFUNCTION(BlueprintCallable, Category = "Target System")
	void TargetActorWithAxisInput(float AxisValue, float Delta = 1.0f);

	// Called when a target is locked off, either if it is out of reach (based on MinimumDistanceToEnable) or behind an Object.
	UPROPERTY(BlueprintAssignable, Category = "Target System")
	FComponentOnTargetLockedOnOff OnTargetLockedOff;

	// Called when a target is locked on
	UPROPERTY(BlueprintAssignable, Category = "Target System")
	FComponentOnTargetLockedOnOff OnTargetLockedOn;

    // Setup the control rotation on Tick when a target is locked on.
    //
    // If not implemented, will fallback to default implementation.
    // If this event is implemented, it lets you control the rotation of the character.
    UPROPERTY(BlueprintAssignable, Category = "Target System")
    FComponentSetRotation OnTargetSetRotation;

	// Returns the reference to currently targeted Actor if any
	UFUNCTION(BlueprintCallable, Category = "Target System")
	AActor* GetLockedOnTargetActor() const;

	// Returns true / false whether the system is targeting an actor
	UFUNCTION(BlueprintCallable, Category = "Target System")
    bool IsLocked() const;

private:

	UPROPERTY()
	AActor* CharacterReference;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UWidgetComponent* TargetLockedOnWidgetComponent;

	UPROPERTY()
	AActor* LockedOnTargetActor;

	FTimerHandle LineOfSightBreakTimerHandle;
	FTimerHandle SwitchingTargetTimerHandle;

	bool bIsBreakingLineOfSight;
	bool bIsSwitchingTarget;
	bool bTargetLocked;
	float ClosestTargetDistance;

    bool bDesireToSwitch;
    float StartRotatingStack;

	TArray<AActor*> GetAllActorsOfClass(TSubclassOf<AActor> ActorClass) const;
	AActor* FindNearestTarget(TArray<AActor*> Actors) const;
	bool LineTrace(FHitResult& HitResult, AActor* OtherActor, TArray<AActor*> ActorsToIgnore = TArray<AActor*>()) const;
	bool LineTraceForActor(AActor* OtherActor, TArray<AActor*> ActorsToIgnore = TArray<AActor*>()) const;
	FRotator GetControlRotationOnTarget(AActor* OtherActor) const;
	void SetControlRotationOnTarget(AActor* TargetActor) const;
	void CreateAndAttachTargetLockedOnWidgetComponent(AActor* TargetActor);
	bool ShouldBreakLineOfSight() const;
	void BreakLineOfSight();
	void ControlRotation(bool ShouldControlRotation) const;
	bool IsInViewport(AActor* TargetActor) const;

	void TargetLockOn(AActor* TargetToLockOn);

	float GetDistanceFromCharacter(AActor* OtherActor) const;
	TArray<AActor*> FindTargetsInRange(TArray<AActor*> ActorsToLook, float RangeMin, float RangeMax) const;
	float GetAngleUsingCameraRotation(AActor* ActorToLook) const;
	float GetAngleUsingCharacterRotation(AActor* ActorToLook) const;
	static FRotator FindLookAtRotation(const FVector Start, const FVector Target);
	void ResetIsSwitchingTarget();

	static bool TargetIsTargetable(AActor* Actor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
