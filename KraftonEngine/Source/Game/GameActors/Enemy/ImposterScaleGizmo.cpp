#include "ImposterScaleGizmo.h"
#include "Game/GameActors/Obstacle/ObstacleActorBase.h"

#include <algorithm>
#include <random>

IMPLEMENT_CLASS(AImposterScaleGizmo, AImposterGizmoActorBase)

namespace {
	constexpr float LerpDuration = 0.5f;

	std::mt19937& RandomEngine()
	{
		static std::mt19937 Engine(std::random_device{}());
		return Engine;
	}

	float RandomScaleFactor()
	{
		std::uniform_real_distribution<float> Distribution(1.05f, 2.0f);
		return Distribution(RandomEngine());
	}
}

void AImposterScaleGizmo::Transform(float DeltaTime) {
	if (!Target) return;

	if (!bTransforming)
	{
		StartScale = Target->GetActorScale();
		const FVector ScaleOffset = GetScaleOffset();
		TargetScale = FVector(
			StartScale.X * ScaleOffset.X,
			StartScale.Y * ScaleOffset.Y,
			StartScale.Z * ScaleOffset.Z);
		Elapsed = 0.0f;
		bTransforming = true;
	}

	Elapsed += (std::max)(0.0f, DeltaTime);
	const float Alpha = (std::min)(Elapsed / LerpDuration, 1.0f);
	Target->SetActorScale(FVector::Lerp(StartScale, TargetScale, Alpha));

	if (Alpha >= 1.0f)
	{
		bTransforming = false;
		Release();
	}
}

// Return a scale within the range [1.05, 2]
FVector AImposterScaleGizmo::GetScaleOffset() const {
	const float ScaleFactor = RandomScaleFactor();
	return FVector(ScaleFactor, ScaleFactor, ScaleFactor);
}
