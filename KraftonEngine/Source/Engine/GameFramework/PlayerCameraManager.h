#pragma once

#include "GameFramework/AActor.h"
#include "Camera/CameraTypes.h"

class UCameraComponent;
class UCameraShakeBase;
class UCameraModifier;
class UClass;

// APlayerCameraManager 대응— PlayerController가 소유하는 카메라 관리 액터.
// ViewTarget이 가진 UCameraComponent를 매 프레임 평가하고, PCM 자체의 modifier list로 view를 추가 변형한 뒤
// 최종 결과를 ViewTarget 카메라에 push한다 (렌더 측은 World->ActiveCamera만 )
class APlayerCameraManager : public AActor
{
public:
	DECLARE_CLASS(APlayerCameraManager, AActor)

	APlayerCameraManager();

	// ViewTarget 관리 
	void SetViewTarget(AActor* NewViewTarget);
	AActor* GetViewTarget() const { return ViewTarget; }
	UCameraComponent* GetViewTargetCamera() const;

	// 매 프레임 호출 — PlayerController가 자신의 Tick에서 호출 
	void UpdateCamera(float DeltaTime);

	// 캐시된 최종 view 정보 
	const FMinimalViewInfo& GetCachedView() const { return CachedView; }
	FVector GetCameraLocation() const { return CachedView.Location; }
	FRotator GetCameraRotation() const { return CachedView.Rotation; }
	float GetCameraFOV() const { return CachedView.FOV; }

	// Modifier 추가 (PCM 자체 modifier — 카메라 컴포넌트 modifier 이후에 적용)
	UCameraModifier* AddCameraModifier(UClass* ModifierClass);
	UCameraModifier* AddCameraModifierByName(const char* ModifierClassName);
	void RemoveCameraModifier(UCameraModifier* Modifier);

	// Convenience — ViewTarget 카메라의 modifier 체인으로 라우팅
	UCameraShakeBase* PlayCameraShake(UClass* ShakeClass, float Scale = 1.0f);
	UCameraShakeBase* PlayCameraShakeByName(const char* ShakeClassName, float Scale = 1.0f);
	void StopCameraShake(UCameraShakeBase* Instance, bool bImmediate = false);
	void StopAllCameraShakes(bool bImmediate = false);
	void PlayHitEffect(float Intensity, float Duration);

private:
	AActor* ViewTarget = nullptr;

	// PCM 자체 modifier list — 카메라 컴포넌트 modifier 이후 view에 적용됨
	TArray<UCameraModifier*> Modifiers;

	// 가장 최근 UpdateCamera 결과
	FMinimalViewInfo CachedView;
};
