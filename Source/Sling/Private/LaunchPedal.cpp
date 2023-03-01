// Fill out your copyright notice in the Description page of Project Settings.


#include "LaunchPedal.h"

#include <valarray>

#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnitConversion.h"

// Sets default values
ALaunchPedal::ALaunchPedal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
	static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> MeshFinder(TEXT("StaticMesh '/Game/Mesh/Pedal.Pedal'"));
	if (MeshFinder.Succeeded())
	{
		Mesh->SetStaticMesh(MeshFinder.Get());
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Begining Pedal mesh not found."));
	}
	Arrow = CreateDefaultSubobject<UArrowComponent>("Arrow");
	Arrow->SetupAttachment(RootComponent);
	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetupAttachment(RootComponent);
	// Param initialization
	LaunchSpeed = 1000.f;
	LaunchAngle = FRotator(30.f, 0.f, 0.f);
	Gravity = 980.f;
	// Arrow->SetRelativeRotation(LaunchAngle);
	Arrow->SetVisibility(true);
	Arrow->SetHiddenInGame(false);
}

// Called when the game starts or when spawned
void ALaunchPedal::BeginPlay()
{
	Super::BeginPlay();
	// Gravity = -1 * GetWorld()->GetGravityZ();
	CollisionBox->SetBoxExtent(Mesh->GetCollisionShape().GetBox());
	CollisionBox->SetRelativeLocation(FVector(0.f, 0.f, 2 * Mesh->GetCollisionShape().GetBox().Z));
	// Gravity = -1 * PlayerRef->GetCharacterMovement()->GravityScale * GetWorld()->GetGravityZ();
	
}

void ALaunchPedal::NotifyActorBeginOverlap(AActor* OtherActor)
{

	ACharacter *Player = Cast<ACharacter>(OtherActor);
	if (Player)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Petal overlap")));
		FVector Force = ComputeLaunchForce();
		PlayerRef->LaunchCharacter(Force, true, true);
	}
}

// Called every frame
void ALaunchPedal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ALaunchPedal::ComputeLaunchForce()
{
	
	FVector Force = LaunchAngle.Vector() * LaunchSpeed;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("arrow vector: %s"), *Arrow->GetForwardVector().ToString()));
	
	// Force = LaunchAngle.RotateVector(Force);

	// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("time interval: %f"), ComputeTimeInterval(LaunchAngle, LaunchSpeed)));
	//
	// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("distance: %f"), ComputeDistance(LaunchAngle, LaunchSpeed)));
	//
	// float distance = ComputeDistance(LaunchAngle, LaunchSpeed);
	// LaunchDistance = distance;
	
	// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("gravity: %f"), PlayerRef->GetCharacterMovement()->GravityScale * GetWorld()->GetGravityZ()));

	UE_LOG(LogTemp, Warning, TEXT("Speed %f Angle %s"), LaunchSpeed, *LaunchAngle.ToString());
	return Force;
}

// float ALaunchPedal::ComputeTimeInterval(FRotator Angle, float Speed)
// {
// 	return 2.f * Speed * std::sin(Angle * PI / 180.f) / Gravity;
// }
//
float ALaunchPedal::ComputeDistance(float Angle, float Speed, float HeightDiff)
{
	Angle = Angle * PI / 180.f;
	// return Speed * Speed * std::sin(2.f * Angle) / Gravity;
	return Speed * std::cos(Angle) * (Speed * std::sin(Angle) + std::sqrt(std::pow(Speed * std::sin(Angle), 2.f) + 2 * Gravity * HeightDiff)) / Gravity;
}

float ALaunchPedal::ComputeAngle(float TimeInterval, float Speed)
{
	return std::asin(TimeInterval * Gravity / 2.f / Speed) * 180 / PI;
}

FQuat ALaunchPedal::TransformAngleToWorldSpace(float Angle)
{
	FRotator RAngle = FRotator(Angle, 0.f, 0.f);
	return GetActorTransform().TransformRotation(RAngle.Quaternion());
}

void ALaunchPedal::SetAngle(FRotator Angle)
{
	LaunchAngle = Angle;
	Arrow->SetWorldRotation(LaunchAngle);
}

void ALaunchPedal::SetSpeed(float Speed)
{
	LaunchSpeed = Speed;
}


