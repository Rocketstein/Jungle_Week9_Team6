#pragma once
#include "Camera/CameraModifier.h"

// 단일 hit-vignette feedback 인스턴스. AddHitEffect를 새로 호출하면 가장 강한 instance가 채택된다 (UE의 ClientPlayCameraShake/HUD feedback 패턴).
struct FHitEffectInstance
{
	float CurrentIntensity = 0.0f;
	float Duration = 1.0f;
};

// post-process hit-feedback을 modifier 체인으로 노출 — view.HitEffectIntensity에 누적.
class UCameraModifier_HitEffect : public UCameraModifier
{
public:
	DECLARE_CLASS(UCameraModifier_HitEffect, UCameraModifier)

	UCameraModifier_HitEffect() = default;

	void AddHitEffect(float Intensity, float Duration);
	void ClearHitEffect();

	float GetCurrentIntensity() const { return Active.CurrentIntensity; }

	bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutView) override;

private:
	FHitEffectInstance Active;
};
