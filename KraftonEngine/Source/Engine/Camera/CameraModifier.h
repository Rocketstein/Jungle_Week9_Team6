#pragma once
#include "Object/Object.h"
#include "Object/ObjectFactory.h"

struct FMinimalViewInfo;

// UE의 UCameraModifier 대응 — 카메라 view를 후처리로 변형하는 추상 베이스.
// PCM 또는 카메라 컴포넌트가 modifier list를 들고 매 프레임 ModifyCamera를 순회 호출한다.
class UCameraModifier : public UObject
{
public:
	DECLARE_CLASS(UCameraModifier, UObject)

	UCameraModifier() = default;

	bool bDisabled = false;
	int32 Priority = 0; // 작은 값이 먼저 적용

	// view를 in-place로 변형. true 반환 시 이후 modifier 체인 중단(UE의 ModifyCamera 반환값과 동일 의도).
	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutView) { (void)DeltaTime; (void)InOutView; return false; }

	// modifier가 카메라/PCM에 등록될 때 호출
	virtual void OnAdded() {}
	// 등록 해제될 때 호출
	virtual void OnRemoved() {}
};
