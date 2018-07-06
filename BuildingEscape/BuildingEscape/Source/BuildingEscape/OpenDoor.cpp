// Copyright Ben Tristem 2016.

#include "BuildingEscape.h"
#include "OpenDoor.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	// Find the owning Actor
	Owner = GetOwner();
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("%s is missing pressure plate!"), *(GetOwner()->GetName()));
	}
}


// Called every frame
void UOpenDoor::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// Poll the trigger volume every frame
	if (GetTotalMassOfActorsOnPlate() > TriggerMass) {
		// then open the door
		onOpen.Broadcast();
	}
	else {
		// else close the door
		onClose.Broadcast();
	}
}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float TotalMass = 0.0f;
	if (!PressurePlate) {
		return TotalMass;
	}

	/// find all the overlapping actors
	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors( OUT OverlappingActors);

	/// iterate through them adding their masses
	for (const AActor* Actor : OverlappingActors) {
		TotalMass += Actor->FindComponentByClass< UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%s is on trigger."), *(Actor->GetName()));
	}
	return TotalMass;
}

