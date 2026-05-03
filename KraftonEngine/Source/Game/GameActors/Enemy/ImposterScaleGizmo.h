#pragma once
#include "ImposterGizmoActorBase.h"

class AImposterScaleGizmo : public AImposterGizmoActorBase {
public:
	DECLARE_CLASS(AImposterScaleGizmo, AImposterGizmoActorBase)

	void Capture(AActor* InTarget)	override;
	void Transform(float DeltaTime) override;

private:
	FVector GetScaleOffset() const;

};