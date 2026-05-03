#pragma once
#include "ImposterGizmoActorBase.h"

class AImposterTranslateGizmo : public AImposterGizmoActorBase {
public:
	DECLARE_CLASS(AImposterTranslateGizmo, AImposterGizmoActorBase)

	void Capture(AActor* InTarget)	override;
	void Transform(float DeltaTime) override;

private:
	FVector GetTranslateOffset() const;

private:
	FVector StartLocation = FVector::ZeroVector;
	FVector TargetLocation = FVector::ZeroVector;
	float Elapsed = 0.0f;
	bool bTransforming = false;
};
