#pragma once
#include "ImposterGizmoActorBase.h"

class AImposterTranslateGizmo : public AImposterGizmoActorBase {
public:
	DECLARE_CLASS(AImposterTranslateGizmo, AImposterGizmoActorBase)

	void Capture(AActor* InTarget)	override;
	void Transform(float DeltaTime) override;

private:
	FVector GetTranslateOffset() const;
};