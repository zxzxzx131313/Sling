// Fill out your copyright notice in the Description page of Project Settings.


#include "SlingHandle.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASlingHandle::ASlingHandle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> MeshFinder(TEXT("StaticMesh '/Game/Mesh/Handle.Handle'"));
	if (MeshFinder.Succeeded())
	{
		Mesh->SetStaticMesh(MeshFinder.Get());
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Begining Handle mesh not found."));
	}
	RootComponent = Mesh;
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	
}

// Called when the game starts or when spawned
void ASlingHandle::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASlingHandle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASlingHandle::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("ON Hit")));
	ACharacter *Player = Cast<ACharacter>(Other);
	if (Player)
	{
		// Player->GetCapsuleComponent()->SetSimulatePhysics(false);
		// Player->GetCapsuleComponent()->SetAllPhysicsLinearVelocity(FVector(0.f, 0.f, 0.f));
		Player->SetActorLocation(GetActorLocation());
		Player->GetCharacterMovement()->DisableMovement();
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Hit actor")));
	}
}


void ASlingHandle::NotifyActorEndOverlap(AActor* OtherActor)
{
	ACharacter *Player = Cast<ACharacter>(OtherActor);
	if (Player)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Overlap end")));
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	}
}

