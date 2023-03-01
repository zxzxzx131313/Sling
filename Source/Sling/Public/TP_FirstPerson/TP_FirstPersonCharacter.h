// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

#include "CableComponent.h"
#include "SlingHandle.h"

#include "TP_FirstPersonCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

// UDELEGATE(BlueprintAuthorityOnly)
// DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSeeAttachPointSignature);

UCLASS(config=Game)
class ATP_FirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* AttachAction;
	
public:
	ATP_FirstPersonCharacter();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;
public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;


protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for attaching input; attach character to sling handle and launch character if need to */
	void Attach(const FInputActionValue& Value);

	/** Called for releasing attaching input */
	void ReleaseAttach(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

public:
	// UPROPERTY(BlueprintAssignable, Category= "EventDispatcher")
	FTimerDynamicDelegate SeeAttachPoint;

	UFUNCTION(BlueprintCallable)
	void FindAttachPoint();

	UFUNCTION(BlueprintCallable)
	FVector CalculateDragForce();

	/** Change cable length with respect to distance between actor and attached sling handle **/
	UFUNCTION(BlueprintCallable)
	void AdjustCableLength();

public:

	UPROPERTY(BlueprintReadWrite)
	FVector DragLocation;

	UPROPERTY(BlueprintReadWrite)
	FVector NewDragLocation;

	UPROPERTY(BlueprintReadWrite)
	FVector OldDragLocation;
	
	UPROPERTY(BlueprintReadWrite)
	FVector EndLocation;
	
	UPROPERTY(BlueprintReadWrite)
	bool CanDrag;

	UPROPERTY(BlueprintReadWrite)
	ASlingHandle *HitTrigger;

	UPROPERTY(BlueprintReadWrite)
	float CableLength;

	UPROPERTY(BlueprintReadWrite)
	float CableShrinkLength;
	
	UPROPERTY(BlueprintReadWrite)
	bool Attached;

	// The furthest distance the sling can reach
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReachDistance;

	// The largest force the sling can exert on player is LaunchBaseForce * Root(2)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaunchBaseForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Debug;

private:
	UCableComponent *Cable;

	bool Triggered;

	ASlingHandle* HitHandle;
};
