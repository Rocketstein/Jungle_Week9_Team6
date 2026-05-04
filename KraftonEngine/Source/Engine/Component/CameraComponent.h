#pragma once
#include "Engine/Core/RayTypes.h"
#include "Object/ObjectFactory.h"
#include "Component/SceneComponent.h"
#include "Math/Matrix.h"
#include "Math/MathUtils.h"
#include "Math/Vector.h"
#include "Collision/ConvexVolume.h"

struct FCameraState
{
	float FOV = 3.14159265358979f / 3.0f;
	float AspectRatio = 16.0f / 9.0f;
	float NearZ = 0.1f;
	float FarZ = 1000.0f;
	float OrthoWidth = 10.0f;
	bool bIsOrthogonal = false;
};

class UCameraShakeBase;
class UCameraModifier;
class UCameraModifier_CameraShake;
class UCameraModifier_HitEffect;
class UClass;

class UCameraComponent : public USceneComponent
{
public:
	DECLARE_CLASS(UCameraComponent, USceneComponent)

	UCameraComponent() = default;

	void BeginPlay() override;
	void EndPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction& ThisTickFunction) override;
	void GetEditableProperties(TArray<FPropertyDescriptor>& OutProps) override;
	void LookAt(const FVector& Target);
	void SetCameraState(const FCameraState& NewState);
	const FCameraState& GetCameraState() const { return CameraState; }

	void SetFOV(float InFOV) { CameraState.FOV = InFOV; }
	void SetOrthoWidth(float InWidth) { CameraState.OrthoWidth = InWidth; }
	void SetOrthographic(bool bOrtho) { CameraState.bIsOrthogonal = bOrtho; }

	void OnResize(int32 Width, int32 Height);

	FMatrix GetViewMatrix() const;
	FMatrix GetProjectionMatrix() const;
	FMatrix GetViewProjectionMatrix() const;
	FConvexVolume GetConvexVolume() const;

	float GetFOV() const { return CameraState.FOV; }
	float GetNearPlane() const { return CameraState.NearZ; }
	float GetFarPlane() const { return CameraState.FarZ; }
	float GetOrthoWidth() const { return CameraState.OrthoWidth; }
	bool IsOrthogonal() const { return CameraState.bIsOrthogonal; }

	FRay DeprojectScreenToWorld(float MouseX, float MouseY, float ScreenWidth, float ScreenHeight);

	// Modifier chain (PCM modifier list와 같은 구조를 카메라 자체에도 부여)
	UCameraModifier* AddCameraModifier(UClass* ModifierClass);
	UCameraModifier* AddCameraModifierByName(const char* ModifierClassName);
	void RemoveCameraModifier(UCameraModifier* Modifier);
	const TArray<UCameraModifier*>& GetCameraModifiers() const { return CameraModifiers; }

	// 모든 modifier를 외부 view에 적용 (PCM이 호출할 진입점) InOutView에 누적.
	void ApplyCameraModifiers(float DeltaTime, struct FMinimalViewInfo& InOutView);

	// Convenience API — UE-style 클래스 기반
	UCameraShakeBase* StartCameraShake(UClass* ShakeClass, float Scale = 1.0f);
	UCameraShakeBase* StartCameraShakeByName(const char* ShakeClassName, float Scale = 1.0f);
	void StopCameraShake(UCameraShakeBase* Instance, bool bImmediate = false);
	void StopAllCameraShakes(bool bImmediate = false);

	void AddHitEffect(float Intensity, float Duration);

	//  Back-compat API  기존 (Intensity, Duration) 호출 호환
	UCameraShakeBase* StartCameraShake(float Intensity, float Duration);

	// Render-side getters 
	// 두 layer 합산 — frame CB로 push되는 최종 hit-effect 강도
	float GetHitEffectIntensity() const { return SelfHitEffectIntensity + ExternalHitEffectIntensity; }

	// PCM이 자기 layer의 추가 offset/HitEffect를 카메라에 push (카메라 자체 modifier 결과와 합산되어 view matrix에 반영됨)
	void SetExternalAdditiveOffsets(const FVector& InLocOffset, const FRotator& InRotOffset, float InAdditionalHitEffect);

	// 카메라 자체 modifier 결과 — PCM이 시드로 사용
	const FVector& GetSelfAdditiveLocation() const { return SelfAdditiveLocationOffset; }
	const FRotator& GetSelfAdditiveRotation() const { return SelfAdditiveRotationOffset; }
	float GetSelfHitEffectIntensity() const { return SelfHitEffectIntensity; }

private:
	UCameraModifier_CameraShake* GetOrCreateCameraShakeModifier();
	UCameraModifier_HitEffect* GetOrCreateHitEffectModifier();

	FCameraState CameraState;

	// Modifier 체인 — 내부에서 매 프레임 ApplyCameraModifiers를 거쳐 누적 결과를 캐시.
	TArray<UCameraModifier*> CameraModifiers;
	UCameraModifier_CameraShake* CachedShakeModifier = nullptr;
	UCameraModifier_HitEffect* CachedHitEffectModifier = nullptr;

	// 카메라 자체 modifier가 만든 offset (TickComponent에서 갱신)
	FVector SelfAdditiveLocationOffset = FVector::ZeroVector;
	FRotator SelfAdditiveRotationOffset = FRotator::ZeroRotator;
	float SelfHitEffectIntensity = 0.0f;

	// PCM이 추가로 적용한 offset (PCM->UpdateCamera에서 갱신)
	FVector ExternalAdditiveLocationOffset = FVector::ZeroVector;
	FRotator ExternalAdditiveRotationOffset = FRotator::ZeroRotator;
	float ExternalHitEffectIntensity = 0.0f;
};
