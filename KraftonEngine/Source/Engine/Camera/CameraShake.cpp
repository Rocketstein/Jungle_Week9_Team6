#include "Camera/CameraShake.h"
#include <cmath>

IMPLEMENT_ABSTRACT_CLASS(UCameraShakeBase, UObject)
IMPLEMENT_CLASS(USinWaveCameraShake, UCameraShakeBase)

void UCameraShakeBase::StartShake(float NewScale)
{
	Scale = NewScale;
	ElapsedTime = 0.0f;
	bStopped = false;
	bImmediateStop = false;
}

void UCameraShakeBase::UpdateAndApplyCameraShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot)
{
	OutLoc = FVector::ZeroVector;
	OutRot = FRotator::ZeroRotator;

	if (IsFinished()) return;

	ElapsedTime += DeltaTime;

	FVector RawLoc = FVector::ZeroVector;
	FRotator RawRot = FRotator::ZeroRotator;
	EvaluateShake(DeltaTime, RawLoc, RawRot);

	OutLoc = RawLoc * Scale;
	OutRot.Pitch = RawRot.Pitch * Scale;
	OutRot.Yaw = RawRot.Yaw * Scale;
	OutRot.Roll = RawRot.Roll * Scale;
}

void UCameraShakeBase::StopShake(bool bImmediate)
{
	bStopped = true;
	bImmediateStop = bImmediate;
	if (bImmediate)
	{
		ElapsedTime = Duration; // 즉시 종료
	}
}

bool UCameraShakeBase::IsFinished() const
{
	if (bImmediateStop) return true;
	return ElapsedTime >= Duration;
}

void USinWaveCameraShake::EvaluateShake(float /*DeltaTime*/, FVector& OutLoc, FRotator& OutRot)
{
	const float Alpha = (Duration > 0.0f) ? (1.0f - (ElapsedTime / Duration)) : 0.0f;
	const float ClampedAlpha = (Alpha < 0.0f) ? 0.0f : Alpha;

	const float Wave = std::sinf(ElapsedTime * Frequency) * Intensity * ClampedAlpha;

	OutLoc = FVector(0.0f, Wave, Wave * 0.5f);
	OutRot.Roll = Wave * 0.2f;
	OutRot.Yaw = Wave * 0.1f;
	OutRot.Pitch = 0.0f;
}
