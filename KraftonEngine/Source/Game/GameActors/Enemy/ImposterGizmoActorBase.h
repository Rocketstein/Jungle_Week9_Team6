#pragma once
#include "EnemyActorBase.h"
#include "Scripting/LuaActorProxy.h"

// "Captures" Actor (Obstacle or the runner player). 
// Obstacles could be captured and move, rotate, and scale arbitraily (in a way that does not coerce game over)
// Player should only be captured by transform gizmo (forced lan switch).
class AImposterGizmoActorBase : public AEnemyActorBase {
public:
	DECLARE_CLASS(AImposterGizmoActorBase, AEnemyActorBase)
	
	virtual void Capture(AActor* InTarget)	= 0;
	virtual void Transform()				= 0;


	void Release() { Target = nullptr; }

	AActor* GetCapturedActor() const { if (Target) return Target; return nullptr; }
	FLuaActorProxy GetCapturedActorProxy() const;

protected:
	virtual ~AImposterGizmoActorBase() = default;

protected:
	AActor* Target = nullptr;

};