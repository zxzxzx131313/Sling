// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedConveyor.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values
ASpeedConveyor::ASpeedConveyor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
	static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> MeshFinder(TEXT("StaticMesh '/Game/Mesh/Conveyor.Conveyor'"));
	if (MeshFinder.Succeeded())
	{
		Mesh->SetStaticMesh(MeshFinder.Get());
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Begining Conveyor mesh not found."));
	}
	Arrow = CreateDefaultSubobject<UArrowComponent>("Arrow");
	Arrow->SetupAttachment(RootComponent);
	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	// CollisionBox->SetBoxExtent(Mesh->GetCollisionShape().GetBox());
	CollisionBox->SetupAttachment(RootComponent);
	// Param initialization
	LaunchSpeed = 2000.f;
}

// Called when the game starts or when spawned
void ASpeedConveyor::BeginPlay()
{
	Super::BeginPlay();
	// CollisionBox->SetBoxExtent(Mesh->GetCollisionShape().GetBox() - Mesh->GetCollisionShape().GetBox() * GetActorForwardVector()/2.f);
	CollisionBox->SetBoxExtent(Mesh->GetCollisionShape().GetBox());
	// CollisionBox->SetRelativeLocation(FVector(0.f, 0.f, 2 * Mesh->GetCollisionShape().GetBox().Z) - Mesh->GetCollisionShape().GetBox() * GetActorForwardVector()/2.f);
	CollisionBox->SetRelativeLocation(FVector(0.f, 0.f, 2 * Mesh->GetCollisionShape().GetBox().Z));
	UE_LOG(LogTemp, Warning, TEXT("Box Extent: %s"), *Mesh->GetCollisionShape().GetBox().ToString());
}

void ASpeedConveyor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Speed Conveyor overlap")));
	ACharacter *Player = Cast<ACharacter>(OtherActor);
	if (Player)
	{
		Force = GetActorForwardVector() * LaunchSpeed;
		UE_LOG(LogTemp, Warning, TEXT("GetActorForwardVector(): %s"), *GetActorForwardVector().ToString());

		Overlapping = true;
		// AddImpluse extremely low frame rate for some reason...
		// PlayerRef->GetCharacterMovement()->AddImpulse();

		PlayerRef->LaunchCharacter(Force, true, true);
		
	}
}

void ASpeedConveyor::NotifyActorEndOverlap(AActor* OtherActor)
{
	Overlapping = false;
}

// Called every frame
void ASpeedConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// DrawDebugBox(GetWorld(), CollisionBox->GetCenterOfMass(), CollisionBox->GetUnscaledBoxExtent(), FColor::Red, true, 10.f, -1.f, 1.f);
	// if (Force != FVector(0))
 //    {
	// 	// UE_LOG(LogTemp, Warning, TEXT("player speed: %s"), *PlayerRef->GetVelocity().ToString());
	// 	if ((PlayerRef->GetVelocity() * PlayerRef->GetActorForwardVector()).Length() < Force.Length())
	// 	{
	// 		PlayerRef->LaunchCharacter(Force, true, true);
	// 		UE_LOG(LogTemp, Warning, TEXT("Force: %s"), *Force.ToString());
	// 		Force = ClampVector(Force - 5.f * GetActorForwardVector(), FVector(0), FVector(2*LaunchSpeed));
	// 	}
	// 	else
	// 	{
	// 		// launched by another conveyor, reset this one
	// 		Force = FVector(0);
	// 	}
 //    }
}

