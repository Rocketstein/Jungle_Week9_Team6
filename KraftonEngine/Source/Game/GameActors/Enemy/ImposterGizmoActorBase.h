#pragma once
#include "EnemyActorBase.h"
#include "Scripting/LuaActorProxy.h"
#include "Component/GizmoComponent.h"

// "Captures" Actor (Obstacles). 
// Obstacles could be captured and move, rotate, and scale arbitraily (in a way that does not coerce game over)
class AImposterGizmoActorBase : public AEnemyActorBase {
public:
	DECLARE_CLASS(AImposterGizmoActorBase, AEnemyActorBase)
	
	virtual void Tick(float DeltaTime) override;
	virtual void Capture(AActor* InTarget);
	virtual void Transform(float DeltaTime) = 0;

	uint8 SetOffsetAxis();
	void Release();

	AActor* GetCapturedActor() const { if (Target) return Target; return nullptr; }
	FLuaActorProxy GetCapturedActorProxy() const;

protected:
	virtual ~AImposterGizmoActorBase() = default;

protected:
	UGizmoComponent* PreviewGizmo = nullptr; 
	AActor* Target			= nullptr;
	float	ActivationDelay = 1.5f;
	float   ElapsedDelay	= 0.f;
	float	Elapsed			= 0.f;
	bool	bTransforming   = false;
	uint8	OffsetAxis		= 0;
};