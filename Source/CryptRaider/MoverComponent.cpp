// Fill out your copyright notice in the Description page of Project Settings.


#include "MoverComponent.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UMoverComponent::UMoverComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMoverComponent::BeginPlay()
{
	Super::BeginPlay();

	OriginalLocation = GetOwner()->GetActorLocation();
	FinalLocation = OriginalLocation + MoveOffset;
	
}


// Called every frame
void UMoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector TargetLocation;
	if(ToggleMove){
		MoveToLocation(OriginalLocation, FinalLocation, DeltaTime);
	} else {
		MoveToLocation(FinalLocation, OriginalLocation, DeltaTime);
	}
}

void UMoverComponent::SetToggleMove(bool state){
	ToggleMove = state;
}


void UMoverComponent::MoveToLocation(FVector From, FVector To, float DeltaTime){
	AActor* PtOwner = GetOwner();
	FVector CurrentLocation = PtOwner->GetActorLocation();
	float Speed = FVector::Distance(From, To) / MoveTime; 
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, To, DeltaTime, Speed);

	PtOwner->SetActorLocation(NewLocation);
}
