#pragma once
#include "ImposterGizmoActorBase.h"

class AImposterRotationGizmo : public AImposterGizmoActorBase {
public:
	DECLARE_CLASS(AImposterRotationGizmo, AImposterGizmoActorBase)

	void Capture(AActor* InTarget)	override;
	void Transform(float DeltaTime) override;
	
private:
	FRotator GetRotationOffset() const;

};