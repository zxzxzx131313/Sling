// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_FirstPerson/TP_FirstPersonCharacter.h"

// #include <shobjidl_core.h>

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Kismet/KismetSystemLibrary.h"
// #include "CrashReportCore/Public/Android/AndroidErrorReport.h"
#include "GameFramework/CharacterMovementComponent.h"

//////////////////////////////////////////////////////////////////////////
// ATP_FirstPersonCharacter

ATP_FirstPersonCharacter::ATP_FirstPersonCharacter()
{
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletonFinder(TEXT("/Script/Engine.SkeletalMesh'/Game/FirstPersonArms/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms'"));
	if(SkeletonFinder.Succeeded())
	{
		Mesh1P->SetSkeletalMeshAsset(SkeletonFinder.Object);
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find skeleton mesh."));
	}
	Mesh1P->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	
	// Set up params
	CanDrag = false;
	Attached = false;
	ReachDistance = 1000.f;
	LaunchBaseForce = 20000.f;
	NewDragLocation = FVector(0.f, 0.f, 0.f);
	Debug = false;
	GetCharacterMovement()->AirControl = 0.f;
	GetCharacterMovement()->GravityScale = 1.5f;

	
	// Setup Cable
	// CableSling = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	// CableSling->bAttachEnd = true;
	// CableSling->bAttachStart = true;
	// CableSling->SetVisibility(false);
	// CableSling->CableWidth = 5.f;
	// CableSling->CableLength = 1.f;
	// CableSling->EndLocation = FVector(0.f, 0.f, 0.f);
	// CableSling->SetAttachEndTo(this, TEXT("Mesh1P"), TEXT("GripPoint"));
	// CableSling->SolverIterations = 16;
	// CableSling->SetupAttachment(GetCapsuleComponent()); 
	
}

void ATP_FirstPersonCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("movement mode %s"), *GetCharacterMovement()->GetMovementName()));
	
	// CableSling->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Setup cable from blueprint
	Cable = (UCableComponent*)GetComponentByClass(UCableComponent::StaticClass());
	if(Cable)
	{
		Cable->bAttachEnd = true;
		Cable->bAttachStart = true;
		Cable->SetVisibility(false);
		
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Cable element not found"));
	}
	
	this->SeeAttachPoint.BindDynamic(this, &ATP_FirstPersonCharacter::FindAttachPoint);
	
	UKismetSystemLibrary::K2_SetTimerDelegate(SeeAttachPoint, 0.2, true);
	
}

void ATP_FirstPersonCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (Debug)
	{
		DrawDebugCapsule(GetWorld(), GetActorLocation(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), GetCapsuleComponent()->GetScaledCapsuleRadius(), GetCapsuleComponent()->GetComponentRotation().Quaternion(), FColor::Green, false
			, 30.f, 3.f);
	}
	if (Attached)
	{
		AdjustCableLength();

		// UE_LOG(LogTemp, Warning, TEXT("velocity %s, speed %f"), *GetVelocity().ToString(), GetVelocity().Length());
		
		// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("velocity %s, speed %f"), *GetVelocity().ToString(), GetVelocity().Length()));
		
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void ATP_FirstPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATP_FirstPersonCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATP_FirstPersonCharacter::Look);

		//Attaching
		EnhancedInputComponent->BindAction(AttachAction,
		ETriggerEvent::Triggered, this,
		&ATP_FirstPersonCharacter::Attach);
		
		EnhancedInputComponent->BindAction(AttachAction,
		ETriggerEvent::Completed, this,
		&ATP_FirstPersonCharacter::ReleaseAttach);
	}
}

void ATP_FirstPersonCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("ON Hit")));
	// // Restore previous hit box collision now we hit a new box
	//
	// if (HitHandle)
	// {
	// 	HitHandle->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	// }
	// HitHandle = Cast<ASlingHandle>(Other);
	// if (HitHandle)
	// {
	// 	GetCharacterMovement()->DisableMovement();
	// 	HitHandle->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	// 	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Hit actor")));
	// }
}

void ATP_FirstPersonCharacter::FindAttachPoint()
{
	FHitResult HitResult;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray;
	// Getting custom object channel
	objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	TArray<AActor*> ignoredActor = TArray<AActor*>();

	FVector SearchStart = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector SearchEnd = GetFirstPersonCameraComponent()->GetComponentLocation() + GetFirstPersonCameraComponent()->GetComponentRotation().Vector()*ReachDistance;

	// Find first hit point
	bool Hit = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), SearchStart, SearchEnd, 10.f, objectTypesArray, false, ignoredActor, EDrawDebugTrace::None, HitResult, true);

	HitTrigger = Cast<ASlingHandle>(HitResult.GetActor());
	if (HitTrigger && Hit)
	{
		DragLocation = HitTrigger->GetActorLocation();
		CanDrag = true;

		// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Hit loacation %s"), *DragLocation.ToString()));
	}
	else
	{
		CanDrag = false;
	}
}

FVector ATP_FirstPersonCharacter::CalculateDragForce()
{
	FVector CurLocation;
	if (NewDragLocation != FVector(0.f, 0.f, 0.f))
	{
		CurLocation = NewDragLocation;
	}else
	{
		CurLocation = GetActorLocation();
	}
	FVector Force =  DragLocation - CurLocation;
	Force.Normalize();
	float DistanceScale = (CurLocation - DragLocation).Length() / ReachDistance;
	if (Debug)
	{
		DrawDebugLine(GetWorld(), CurLocation, DragLocation, FColor::Red, true, 30.f, -1, 5.f);
        UE_LOG(LogTemp, Warning, TEXT("Distance scale: %f"), DistanceScale);
        GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Red, FString::Printf(TEXT("Force: %s"), *Force.ToString()));
	}

	Force = Force * DistanceScale * LaunchBaseForce;
	return Force;
}

void ATP_FirstPersonCharacter::AdjustCableLength()
{
	CableLength = (GetActorLocation() - NewDragLocation).Length() - 400.f;
	Cable->CableLength = CableLength;
}


void ATP_FirstPersonCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ATP_FirstPersonCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATP_FirstPersonCharacter::Attach(const FInputActionValue& Value)
{
	if (!Triggered)
	{
		Triggered = true;
		// Attach Point find
		if (CanDrag)
        {
        	// Do nothing if still aimed at same object
        	if (DragLocation != NewDragLocation)
        	{
        		FVector Force = CalculateDragForce();
        		GetCharacterMovement()->SetDefaultMovementMode();
        		NewDragLocation = DragLocation;
        		FTransform SocketTrans = Mesh1P->GetSocketTransform(TEXT("Weapon_R"), ERelativeTransformSpace::RTS_Actor);
    
        		EndLocation = SocketTrans.GetLocation();
        	
        		Cable->SetWorldLocation(NewDragLocation);
        	
        		CableLength = (GetActorLocation() - NewDragLocation).Length() - 400.f;
        		
        		Cable->EndLocation = EndLocation;
        		Cable->CableLength = CableLength;
        		Attached = true;
        		Cable->SetVisibility(true);
        	
        		// GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Red, FString::Printf(TEXT("length %f"), CableLength));
        		GetCharacterMovement()->GravityScale = 0.f;
        		LaunchCharacter(Force, true, true);
        	}
        }else
        {
        	// Aimed at non-attached point, release player
        	// TODO: will automatically release if touching ground
        	GetCharacterMovement()->GravityScale = 1.f;
        	GetCharacterMovement()->SetDefaultMovementMode();
        	Cable->SetWorldLocation(GetActorLocation());
        	Cable->SetVisibility(false);

        	// clear history of attach point
        	NewDragLocation = FVector(0.f, 0.f, 0.f);

        	// Restore box collision that was set to none for passing through without blocking
        	if (HitHandle)
        	{
        		HitHandle->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
        	}
        	
        }
	}
	
}

void ATP_FirstPersonCharacter::ReleaseAttach(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Released")));
	Triggered = false;
}