#pragma once

#include "Math/Vector.h"
#include "Math/Rotator.h"

// 카메라 modifier 체인을 통과하는 view 정보 — UE의 FMinimalViewInfo 대응.
// 각 modifier는 InOut 으로 받아 필요한 필드만 변형한다.
struct FMinimalViewInfo
{
	FVector Location = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;

	float FOV = 3.14159265358979f / 3.0f;
	float AspectRatio = 16.0f / 9.0f;
	float NearZ = 0.1f;
	float FarZ = 1000.0f;
	float OrthoWidth = 10.0f;
	bool bIsOrthogonal = false;

	// Post-process intensities accumulated by modifiers
	float HitEffectIntensity = 0.0f;
};
