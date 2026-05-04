#pragma once
#include "Object/Object.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "Object/ObjectFactory.h"

// UE의 UCameraShakeBase + UCameraShakePattern를 합친 구조.
// 인스턴스가 lifecycle(Start/Stop/IsFinished)을 갖고 매 프레임 위치/회전 오프셋을 출력한다.
// Scale은 인스턴스별 강도 배율 — UE의 CameraShake Scale과 동일.
class UCameraShakeBase : public UObject
{
public:
	DECLARE_CLASS(UCameraShakeBase, UObject)

	UCameraShakeBase() = default;

	// 인스턴스별 강도 배율 (1.0 = 클래스 기본값 그대로)
	float Scale = 1.0f;

	// 클래스 기본 파라미터 — 서브클래스가 override 하거나 기본값 그대로 사용
	float Duration = 0.5f;
	float Intensity = 1.0f;

	// 런타임 상태
	float ElapsedTime = 0.0f;
	bool bStopped = false;
	bool bImmediateStop = false;

	// === Lifecycle ===
	virtual void StartShake(float NewScale = 1.0f);
	virtual void UpdateAndApplyCameraShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot);
	virtual void StopShake(bool bImmediate);
	virtual bool IsFinished() const;

protected:
	// 서브클래스 override 포인트 — 누적 시간 기준으로 location/rotation offset을 계산.
	// Scale 배율은 base가 곱해 적용하므로 서브클래스는 raw 값을 출력.
	virtual void EvaluateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot) {}
};

// 사인 곡선 기반 셰이크 — 가장 단순한 구현.
class USinWaveCameraShake : public UCameraShakeBase
{
public:
	DECLARE_CLASS(USinWaveCameraShake, UCameraShakeBase)
	USinWaveCameraShake() = default;

	float Frequency = 25.0f;

protected:
	void EvaluateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot) override;
};
