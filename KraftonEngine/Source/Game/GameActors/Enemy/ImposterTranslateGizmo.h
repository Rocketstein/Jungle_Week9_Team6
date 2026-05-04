#pragma once
#include "ImposterGizmoActorBase.h"

class AImposterTranslateGizmo : public AImposterGizmoActorBase {
public:
	DECLARE_CLASS(AImposterTranslateGizmo, AImposterGizmoActorBase)
	void Transform(float DeltaTime) override;

private:
	FVector GetTranslateOffset() const;

private:
	FVector StartLocation  = FVector::ZeroVector;
	FVector TargetLocation = FVector::ZeroVector;
};
