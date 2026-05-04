#include "ImposterGizmoActorBase.h"
#include "Game/GameActors/Obstacle/ObstacleActorBase.h"


DEFINE_CLASS(AImposterGizmoActorBase, AEnemyActorBase)

void AImposterGizmoActorBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (!Target) return;
	if (ElapsedDelay < ActivationDelay) ElapsedDelay += DeltaTime;
	if (ElapsedDelay >= ActivationDelay) Transform(DeltaTime);
}

void AImposterGizmoActorBase::Capture(AActor* InActor) {
	if (!InActor) return;
	if (!InActor->IsA<AObstacleActorBase>()) return;
	Target = InActor;
	Elapsed = 0.0f;
	bTransforming = false;

	PreviewGizmo = AddComponent<UGizmoComponent>();
}

FLuaActorProxy AImposterGizmoActorBase::GetCapturedActorProxy() const {
	FLuaActorProxy Proxy;
	Proxy.Actor = Target;
	return Proxy;
}
