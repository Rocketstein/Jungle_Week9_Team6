#include "CapsuleComponent.h"
#include "GameFramework/World.h"

IMPLEMENT_CLASS(UCapsuleComponent, UShapeComponent)

void UCapsuleComponent::DrawDebugShape(UWorld* InWorld) const {
	if (CapsuleRadius <= 0.f || CapsuleHalfHeight < 0.f) return;

	FVector Center    = GetWorldLocation();
	FVector Up        = GetUpVector().Normalized();
	FVector Fwd       = GetForwardVector().Normalized();
	FVector Right     = GetRightVector().Normalized();
	FVector TopCenter = Center + Up * CapsuleHalfHeight;
	FVector BotCenter = Center - Up * CapsuleHalfHeight;
	FVector NegUp     = Up * -1.f;

	constexpr uint32 Segments     = 24;
	constexpr uint32 HalfSegments = Segments / 2;

	// Junction rings at top and bottom
	DrawDebugRing(TopCenter, CapsuleRadius, Fwd, Right, Segments,     false, InWorld);
	DrawDebugRing(BotCenter, CapsuleRadius, Fwd, Right, Segments,     false, InWorld);

	// Top hemisphere
	DrawDebugRing(TopCenter, CapsuleRadius, Fwd,  Up,    HalfSegments, true,  InWorld);
	DrawDebugRing(TopCenter, CapsuleRadius, Right, Up,    HalfSegments, true,  InWorld);

	// Bottom hemisphere
	DrawDebugRing(BotCenter, CapsuleRadius, Fwd,  NegUp, HalfSegments, true,  InWorld);
	DrawDebugRing(BotCenter, CapsuleRadius, Right, NegUp, HalfSegments, true,  InWorld);

	// Body lines
	auto& Scene = InWorld->GetScene();
	Scene.AddDebugLine(TopCenter + Fwd   * CapsuleRadius, BotCenter + Fwd   * CapsuleRadius, ShapeColor);
	Scene.AddDebugLine(TopCenter - Fwd   * CapsuleRadius, BotCenter - Fwd   * CapsuleRadius, ShapeColor);
	Scene.AddDebugLine(TopCenter + Right * CapsuleRadius, BotCenter + Right * CapsuleRadius, ShapeColor);
	Scene.AddDebugLine(TopCenter - Right * CapsuleRadius, BotCenter - Right * CapsuleRadius, ShapeColor);
}