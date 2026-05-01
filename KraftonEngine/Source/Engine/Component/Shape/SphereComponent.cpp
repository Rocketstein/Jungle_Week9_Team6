#include "SphereComponent.h"
#include "GameFramework/World.h"

IMPLEMENT_CLASS(USphereComponent, UShapeComponent)

void USphereComponent::DrawDebugShape(UWorld* InWorld) const {
	if (SphereRadius <= 0.f) return;
	constexpr uint32 Segments = 24;
	
	FVector Center = GetWorldLocation();

	DrawDebugRing(Center, SphereRadius, FVector(1, 0, 0), FVector(0, 1, 0), Segments, false, InWorld);
	DrawDebugRing(Center, SphereRadius, FVector(1, 0, 0), FVector(0, 0, 1), Segments, false, InWorld);
	DrawDebugRing(Center, SphereRadius, FVector(0, 1, 0), FVector(0, 0, 1), Segments, false, InWorld);
}