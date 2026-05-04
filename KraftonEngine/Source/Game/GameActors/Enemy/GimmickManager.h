#pragma once

#include "Game/GameActors/Enemy/GimmickActorBase.h"

class UWorld;

// TODO: Add more gimmicks.
enum class EGimmickType
{
	TranslateGizmo,
	RotationGizmo,
	ScaleGizmo,
	Count,
};

class FGimmickManager
{
public:
	FGimmickManager() = default;

	AGimmickActorBase* SpawnGimmickActor(UWorld* World, EGimmickType Gimmick);
	AGimmickActorBase* TrySpawnRandomGimmick(UWorld* World, float SpawnChance);

private:
	AActor* FindRandomObstacleTarget(UWorld* World) const;
	EGimmickType SelectRandomGimmickType() const;
};
