#include "ImposterRotationGizmo.h"
#include "Game/GameActors/Obstacle/ObstacleActorBase.h"

#include <algorithm>
#include <random>

IMPLEMENT_CLASS(AImposterRotationGizmo, AImposterGizmoActorBase)

namespace {
	constexpr float LerpDuration = 0.5f;

	std::mt19937& RandomEngine()
	{
		static std::mt19937 Engine(std::random_device{}());
		return Engine;
	}

	float RandomAngle()
	{
		std::uniform_int_distribution<int> Distribution(0, 3);
		return static_cast<float>(Distribution(RandomEngine()) + 1) * 45.0f;
	}

	int RandomAxis()
	{
		std::uniform_int_distribution<int> Distribution(0, 2);
		return Distribution(RandomEngine());
	}
}

void AImposterRotationGizmo::Transform(float DeltaTime) {
	if (!Target) return;

	if (!bTransforming)
	{
		StartRotation = Target->GetActorRotation();
		TargetRotation = (StartRotation + GetRotationOffset()).GetClamped();
		Elapsed = 0.0f;
		bTransforming = true;
	}

	Elapsed += (std::max)(0.0f, DeltaTime);
	const float Alpha = (std::min)(Elapsed / LerpDuration, 1.0f);
	const FRotator NewRotation(
		StartRotation.Pitch + (TargetRotation.Pitch - StartRotation.Pitch) * Alpha,
		StartRotation.Yaw + (TargetRotation.Yaw - StartRotation.Yaw) * Alpha,
		StartRotation.Roll + (TargetRotation.Roll - StartRotation.Roll) * Alpha);
	Target->SetActorRotation(NewRotation.GetClamped());

	if (Alpha >= 1.0f)
	{
		bTransforming = false;
		Release();
	}
}

FRotator AImposterRotationGizmo::GetRotationOffset() const {
	const float Angle = RandomAngle();
	switch (RandomAxis())
	{
	case 0:
		return FRotator(Angle, 0.0f, 0.0f);
	case 1:
		return FRotator(0.0f, Angle, 0.0f);
	default:
		return FRotator(0.0f, 0.0f, Angle);
	}
}
