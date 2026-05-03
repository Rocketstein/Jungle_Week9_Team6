
#include "ImposterTranslateGizmo.h"
#include "Game/Player/Runner.h"
#include "Game/GameActors/Obstacle/ObstacleActorBase.h"

IMPLEMENT_CLASS(AImposterTranslateGizmo, AImposterGizmoActorBase)

namespace {
	constexpr float LerpDuration = 0.5f;
}

void AImposterTranslateGizmo::Capture(AActor* InActor) {
	if (!InActor) return;
	if (!InActor->IsA<ARunner>() && !InActor->IsA<AObstacleActorBase>()) return;
	Target = InActor;
}

void AImposterTranslateGizmo::Transform(float DeltaTime) {
	FVector TranslateOffset = GetTranslateOffset();

	// TODO: Interpolate to offset location instead of teleporting
	Target->AddActorWorldOffset(TranslateOffset);
}

FVector AImposterTranslateGizmo::GetTranslateOffset() const {
	FVector Out;

	return Out;
}