
#include "ImposterTranslateGizmo.h"
#include "Game/Player/Runner.h"
#include "Game/GameActors/Obstacle/ObstacleActorBase.h"

#include <algorithm>
#include <random>

IMPLEMENT_CLASS(AImposterTranslateGizmo, AImposterGizmoActorBase)

namespace {
	constexpr float LerpDuration = 0.5f;

	std::mt19937& RandomEngine()
	{
		static std::mt19937 Engine(std::random_device{}());
		return Engine;
	}

	int RandomSign()
	{
		std::uniform_int_distribution<int> Distribution(0, 1);
		return Distribution(RandomEngine()) == 0 ? -1 : 1;
	}

	int RandomAxis()
	{
		std::uniform_int_distribution<int> Distribution(0, 2);
		return Distribution(RandomEngine());
	}

	float RandomOffset()
	{
		std::uniform_int_distribution<int> Distribution(0, 2);
		return Distribution(RandomEngine());
	}
}

void AImposterTranslateGizmo::Capture(AActor* InActor) {
	if (!InActor) return;
	if (!InActor->IsA<AObstacleActorBase>()) return;
	Target = InActor;
	Elapsed = 0.0f;
	bTransforming = false;
}

void AImposterTranslateGizmo::Transform(float DeltaTime) {
	if (!Target) return;

	if (!bTransforming)
	{
		StartLocation = Target->GetActorLocation();
		TargetLocation = StartLocation + GetTranslateOffset();
		Elapsed = 0.0f;
		bTransforming = true;
	}

	Elapsed += (std::max)(0.0f, DeltaTime);
	const float Alpha = (std::min)(Elapsed / LerpDuration, 1.0f);
	Target->SetActorLocation(FVector::Lerp(StartLocation, TargetLocation, Alpha));

	if (Alpha >= 1.0f)
	{
		bTransforming = false;
		Release();
	}
}

FVector AImposterTranslateGizmo::GetTranslateOffset() const {
	switch (RandomAxis())
	{
	case 0:
		return FVector(RandomOffset(), 0.0f, 0.0f);
	case 1:
		return FVector(0.0f, RandomOffset(), 0.0f);
	default:
		return FVector(0.0f, 0.0f, RandomOffset());
	}
}
