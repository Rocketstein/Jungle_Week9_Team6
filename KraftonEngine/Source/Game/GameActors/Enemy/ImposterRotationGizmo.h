#pragma once
#include "ImposterGizmoActorBase.h"

class AImposterRotationGizmo : public AImposterGizmoActorBase {
public:
	DECLARE_CLASS(AImposterRotationGizmo, AImposterGizmoActorBase)

	void Capture(AActor* InTarget)	override;
	void Transform(float DeltaTime) override;
	
private:
	FRotator GetRotationOffset() const;

private:
	FRotator StartRotation = FRotator::ZeroRotator;
	FRotator TargetRotation = FRotator::ZeroRotator;
	float Elapsed = 0.0f;
	bool bTransforming = false;
};
