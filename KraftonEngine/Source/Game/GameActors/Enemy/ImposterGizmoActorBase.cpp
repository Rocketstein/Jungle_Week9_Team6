#include "ImposterGizmoActorBase.h"

DEFINE_CLASS(AImposterGizmoActorBase, AEnemyActorBase)

void AImposterGizmoActorBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (!Target) return;
	if (ElapsedDelay < ActivationDelay) ElapsedDelay += DeltaTime;
	if (ElapsedDelay >= ActivationDelay) Transform(DeltaTime);
}

FLuaActorProxy AImposterGizmoActorBase::GetCapturedActorProxy() const {
	FLuaActorProxy Proxy;
	Proxy.Actor = Target;
	return Proxy;
}
