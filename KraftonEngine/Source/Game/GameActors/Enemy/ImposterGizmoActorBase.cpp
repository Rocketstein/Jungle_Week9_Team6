#include "ImposterGizmoActorBase.h"
#include "Game/GameActors/Obstacle/ObstacleActorBase.h"

#include <algorithm>
#include <random>

DEFINE_CLASS(AImposterGizmoActorBase, AEnemyActorBase)

namespace {
	std::mt19937& RandomEngine()
	{
		static std::mt19937 Engine(std::random_device{}());
		return Engine;
	}
}

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

uint8 AImposterGizmoActorBase::SetOffsetAxis() {
	std::uniform_int_distribution<int> Distribution(0, 2);
	OffsetAxis = Distribution(RandomEngine());
	return OffsetAxis;
}

FLuaActorProxy AImposterGizmoActorBase::GetCapturedActorProxy() const {
	FLuaActorProxy Proxy;
	Proxy.Actor = Target;
	return Proxy;
}

void AImposterGizmoActorBase::Release() {
	PreviewGizmo->SetSelectedAxis(-1);
	PreviewGizmo->Deactivate();
	Target = nullptr;
	EndPlay();
	UObjectManager::Get().DestroyObject(this);
}