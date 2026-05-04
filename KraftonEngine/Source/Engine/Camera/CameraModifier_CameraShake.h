#pragma once
#include "Camera/CameraModifier.h"
#include "Core/CoreTypes.h"

class UCameraShakeBase;
class UClass;

// UCameraModifier_CameraShake 대응 — 활성 셰이크들을 관리하고 매 프레임 누적 결과를 view에 적용
class UCameraModifier_CameraShake : public UCameraModifier
{
public:
	DECLARE_CLASS(UCameraModifier_CameraShake, UCameraModifier)

	UCameraModifier_CameraShake() = default;

	// 클래스 기반 시작 — StartCameraShake(TSubclassOf, Scale)와 동일한 의미
	UCameraShakeBase* AddCameraShake(UClass* ShakeClass, float Scale = 1.0f);
	UCameraShakeBase* AddCameraShakeByName(const char* ShakeClassName, float Scale = 1.0f);

	// 특정 인스턴스 정지 bImmediate=true면 즉시 제거, false면 fade-out으로 자연 종료(현 구현은 즉시 종료와 동일하지만 의미 보존)
	void RemoveCameraShake(UCameraShakeBase* ShakeInstance, bool bImmediate = false);
	void RemoveAllCameraShakes(bool bImmediate = false);

	int32 GetActiveShakeCount() const { return static_cast<int32>(ActiveShakes.size()); }

	// UCameraModifier override
	bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutView) override;
	void OnRemoved() override;

private:
	TArray<UCameraShakeBase*> ActiveShakes;
};
