#include "ImposterScaleGizmo.h"
#include "Game/GameActors/Obstacle/ObstacleActorBase.h"

IMPLEMENT_CLASS(AImposterScaleGizmo, AImposterGizmoActorBase)

namespace {
	constexpr float LerpDuration = 0.5f;
}

void AImposterScaleGizmo::Capture(AActor* InActor) {
	if (!InActor) return;
	if (!InActor->IsA<AObstacleActorBase>()) return;

	Target = InActor;
}

void AImposterScaleGizmo::Transform(float DeltaTime) {
	// TODO: Use lerp to gradually scale the target actor
}

// Return a scale within the range [1.05, 2]
FVector AImposterScaleGizmo::GetScaleOffset() const {
	FVector Out;

	return Out;
}