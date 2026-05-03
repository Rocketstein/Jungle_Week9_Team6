#include "ImposterRotationGizmo.h"
#include "Game/GameActors/Obstacle/ObstacleActorBase.h"

IMPLEMENT_CLASS(AImposterRotationGizmo, AImposterGizmoActorBase)

void AImposterRotationGizmo::Capture(AActor* InActor) {
	if (!InActor) return;
	if (!InActor->IsA<AObstacleActorBase>()) return;
	
	Target = InActor;
}

void AImposterRotationGizmo::Transform(float DeltaTime) {
	if (!Target) return;
	FRotator RotationOffset = GetRotationOffset();
	
	// Attaches a rotator movement component to Target Root Component
}

FRotator AImposterRotationGizmo::GetRotationOffset() const {
	FRotator Out;

	return Out;
}