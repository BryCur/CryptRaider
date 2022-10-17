// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabberComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UGrabberComponent::UGrabberComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabberComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UGrabberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// if the player is holding something, we update its location and rotation
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	if(!PhysicsHandleComponentExists(PhysicsHandle)){
		return;
	} else if (PhysicsHandle->GetGrabbedComponent() != nullptr) {
		FVector TargetLocation = GetComponentLocation() + GetForwardVector()*HoldDistance;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}

}

// release whatever the player is holding
void UGrabberComponent::Release(){
	// get and assert the physics handle component
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	if(!PhysicsHandleComponentExists(PhysicsHandle)){
		return;
	}
	
	// release the object held
	if(PhysicsHandle->GetGrabbedComponent() != nullptr){
		PhysicsHandle->GetGrabbedComponent()->WakeAllRigidBodies();
		PhysicsHandle->GetGrabbedComponent()->GetOwner()->Tags.Remove("Grabbed");
		PhysicsHandle->ReleaseComponent();
	}
}

// sweep trace in front of the player to see if something can be held, if something is hit then grab it
void UGrabberComponent::Grab(){

	// get and assert physics handle component
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	if(!PhysicsHandleComponentExists(PhysicsHandle)){
		return;
	}


	FHitResult HitResult;
	bool HasHit = GetGrabbableInReach(HitResult); // sweep and check if something is hit
	if(HasHit){
		GrabComponentHit(HitResult); // grab whatever was hit first
	}
}


// retrieve the Physics Handle component and return a boolean representing its existance
bool UGrabberComponent::PhysicsHandleComponentExists(UPhysicsHandleComponent* &ptrOutPhysicsHandle){
	
	ptrOutPhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if( ptrOutPhysicsHandle == nullptr){
		UE_LOG(LogTemp, Error, TEXT("No Physics Handle Component Found"));
	} else {
		// UE_LOG(LogTemp, Display, TEXT("Should be ok"));
	}

	return ptrOutPhysicsHandle != nullptr;
}

// perform the sweep tracing, store the result in the parameter, and return a boolean stating if something was hit
bool UGrabberComponent::GetGrabbableInReach(FHitResult &OutHitResult) const{
	FVector Start = GetComponentLocation();

	FVector End = Start + GetForwardVector()*GrabRange; 

	UWorld* World = GetWorld();

	// DrawDebugSphere(World, End, 10, 10, FColor::Green, false, 2);

	// sweeping
	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	return World->SweepSingleByChannel(OutHitResult, 
		Start, End, 
		FQuat::Identity, 
		ECC_GameTraceChannel2,
		Sphere
	);

}

// Grab whatever is in the parameter
void UGrabberComponent::GrabComponentHit(FHitResult &HitResult){
		UPhysicsHandleComponent* PhysicsHandle = nullptr;
		if(!PhysicsHandleComponentExists(PhysicsHandle)){
			return;
		}

		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		HitResult.GetActor()->Tags.Add("Grabbed");
		HitResult.GetActor()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		HitComponent->SetSimulatePhysics(true);
		HitComponent->WakeAllRigidBodies();

		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitComponent, 
			NAME_None,
			HitResult.ImpactPoint,
			GetComponentRotation()
		);
}