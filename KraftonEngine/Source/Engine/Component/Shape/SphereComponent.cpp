#include "SphereComponent.h"
#include "GameFramework/World.h"

IMPLEMENT_CLASS(USphereComponent, UShapeComponent)

void USphereComponent::DrawDebugShape(UWorld* InWorld) {
	if (SphereRadius <= 0.f) return;
	constexpr int32 Segments = 24;
	const float Step = 2.0f * FMath::Pi / static_cast<float>(Segments);
	FVector Center = GetWorldLocation();
	FVector AxisA  = FVector::ForwardVector;
	FVector AxisB  = FVector::RightVector;
	FVector Prev = Center + AxisA * SphereRadius;

	for (int32 Index = 1; Index <= Segments; ++Index)
	{
		const float Angle = Step * static_cast<float>(Index);
		const FVector Next = Center + (AxisA * cosf(Angle) + AxisB * sinf(Angle)) * SphereRadius;
		InWorld->GetScene().AddDebugLine(Prev, Next, ShapeColor);
		Prev = Next;
	}

}