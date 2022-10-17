// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"

// Sets default values for this component's properties
UTriggerComponent::UTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();

    if(SecretDoor ==nullptr){
		UE_LOG(LogTemp, Error, TEXT("Trigger should have a reference to a secret door actor |||| ref: %u"), SecretDoor);
	} else {
		UMoverComponent* test=(UMoverComponent*) SecretDoor->GetComponentByClass(UMoverComponent::StaticClass());
		// UE_LOG(LogTemp, Display, TEXT("test: %u"), test);
		SetMover((UMoverComponent*) SecretDoor->GetComponentByClass(UMoverComponent::StaticClass()));

		if(Mover == nullptr){
			UE_LOG(LogTemp, Error, TEXT("Secret door should have a mover component attached"));
		}
	}
	
}

void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* KeyActor = GetAcceptableActor();
	if(KeyActor != nullptr && !KeyActor->ActorHasTag("Grabbed")){
		// UE_LOG(LogTemp, Display, TEXT("unlocking"));
		UPrimitiveComponent* rootComponent = Cast<UPrimitiveComponent>(KeyActor->GetRootComponent());
		if(rootComponent !=nullptr){
			// attach the key actor to the trigger component
			KeyActor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
			rootComponent->SetSimulatePhysics(false);

			// put the key actor in a decent position on the trigger
			FVector OwnerLocation = GetOwner()->GetActorLocation();
			KeyActor->SetActorLocation(OwnerLocation + KeyPositionOffset);
			KeyActor->SetActorRotation(GetOwner()->GetActorRotation());
		}
		Mover->SetToggleMove(true);
	}
	else {
		// UE_LOG(LogTemp, Display, TEXT("unlocking"));
		Mover->SetToggleMove(false);
	}
}

// retrieve the pointer to the actor who should trigger the mechanism. nullptr if no such actor found.
AActor* UTriggerComponent::GetAcceptableActor() const{
	TArray<AActor*> Actors;
	GetOverlappingActors(Actors);

	for(AActor* PtrActor : Actors){
		if(PtrActor->ActorHasTag(KeyTag)){
			
			return PtrActor;
		}
	}

	return nullptr;
}

void UTriggerComponent::SetMover(UMoverComponent* NewMover){
	Mover = NewMover;
}