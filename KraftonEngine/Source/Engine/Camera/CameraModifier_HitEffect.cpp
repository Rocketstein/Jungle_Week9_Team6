#include "Camera/CameraModifier_HitEffect.h"
#include "Camera/CameraTypes.h"

IMPLEMENT_CLASS(UCameraModifier_HitEffect, UCameraModifier)

void UCameraModifier_HitEffect::AddHitEffect(float Intensity, float Duration)
{
	// 기존보다 강한 hit가 들어오면 덮어쓰고, 약한 hit은 무시 — 연사 시 잔상 깜빡임 방지
	if (Intensity >= Active.CurrentIntensity)
	{
		Active.CurrentIntensity = Intensity;
		Active.Duration = (Duration > 0.0f) ? Duration : 1.0f;
	}
}

void UCameraModifier_HitEffect::ClearHitEffect()
{
	Active.CurrentIntensity = 0.0f;
}

bool UCameraModifier_HitEffect::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutView)
{
	if (bDisabled) return false;

	if (Active.CurrentIntensity > 0.0f)
	{
		Active.CurrentIntensity -= (1.0f / Active.Duration) * DeltaTime;
		if (Active.CurrentIntensity < 0.0f) Active.CurrentIntensity = 0.0f;
	}

	InOutView.HitEffectIntensity += Active.CurrentIntensity;
	return false;
}
