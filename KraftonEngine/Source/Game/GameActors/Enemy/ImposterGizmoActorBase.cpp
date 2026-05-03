#include "ImposterGizmoActorBase.h"

DEFINE_CLASS(AImposterGizmoActorBase, AEnemyActorBase)

FLuaActorProxy AImposterGizmoActorBase::GetCapturedActorProxy() const {
	FLuaActorProxy Proxy;
	return Proxy;
}