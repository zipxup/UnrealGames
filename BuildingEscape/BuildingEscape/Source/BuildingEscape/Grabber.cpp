// Copyright Ben Tristem 2016.

#include "BuildingEscape.h"
#include "Grabber.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

/// find attached physics handle
void UGrabber::FindPhysicsHandleComponent()
{	
	PhysicsHandle = GetOwner()->FindComponentByClass< UPhysicsHandleComponent >();
	if (PhysicsHandle == nullptr) {
		// physics handle is not found
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle."), *(GetOwner()->GetName()));
	}
}

/// find attached input component (only appears at runtime)
void UGrabber::SetupInputComponent()
{	
	InputComponent = GetOwner()->FindComponentByClass< UInputComponent >();
	if (InputComponent) {
		/// bind input axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s missing input component."), *(GetOwner()->GetName()));
	}
}

void UGrabber::Grab() {
	/// Line trace and see if reach any actor with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent *ComponentToGrab = HitResult.GetComponent(); // gets the mesh component

	//if we hit something then attach a physics handle
	if (PhysicsHandle && ComponentToGrab) {
		/// attach physics handle
		PhysicsHandle->GrabComponent(
			ComponentToGrab, 
			NAME_None, // no bones needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true // allow rotation
		);
	}		
}

void UGrabber::Release() {
	// release physics handle
	if (PhysicsHandle) {
		PhysicsHandle->ReleaseComponent();
	}	
}

// Called every frame
void UGrabber::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	FVector LineTraceEnd = GetReachLineEnd();
	//if the physics handle is attached
	if (PhysicsHandle && PhysicsHandle->GrabbedComponent) {
		//move the object that we're holding
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}	
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	/// setup query parameters
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	/// ray-cast out to reach distance
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);
	return HitResult;
}

FVector UGrabber::GetReachLineStart() {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
	return PlayerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd() {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

