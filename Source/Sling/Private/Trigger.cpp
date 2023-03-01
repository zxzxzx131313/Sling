// Fill out your copyright notice in the Description page of Project Settings.


#include "Trigger.h"

// Sets default values
ATrigger::ATrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATrigger::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerRef = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

// Called every frame
void ATrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

