#include "GimmickManager.h"

#include "Game/GameActors/Enemy/ImposterRotationGizmo.h"
#include "Game/GameActors/Enemy/ImposterScaleGizmo.h"
#include "Game/GameActors/Enemy/ImposterTranslateGizmo.h"
#include "Game/GameActors/Obstacle/ObstacleActorBase.h"
#include "Game/Map/MapRandom.h"
#include "GameFramework/World.h"

AGimmickActorBase* FGimmickManager::TrySpawnRandomGimmick(UWorld* World, float SpawnChance)
{
	if (!World || !MapRandom::Chance(SpawnChance))
	{
		return nullptr;
	}

	return SpawnGimmickActor(World, SelectRandomGimmickType());
}

AGimmickActorBase* FGimmickManager::SpawnGimmickActor(UWorld* World, EGimmickType Gimmick)
{
	if (!World)
	{
		return nullptr;
	}

	switch (Gimmick)
	{
	case EGimmickType::TranslateGizmo:
	{
		AActor* Target = FindRandomObstacleTarget(World);
		if (!Target)
		{
			return nullptr;
		}

		AImposterTranslateGizmo* Actor = World->SpawnActor<AImposterTranslateGizmo>();
		if (Actor)
		{
			Actor->Capture(Target);
		}
		return Actor;
	}
	case EGimmickType::RotationGizmo:
	{
		AActor* Target = FindRandomObstacleTarget(World);
		if (!Target)
		{
			return nullptr;
		}

		AImposterRotationGizmo* Actor = World->SpawnActor<AImposterRotationGizmo>();
		if (Actor)
		{
			Actor->Capture(Target);
		}
		return Actor;
	}
	case EGimmickType::ScaleGizmo:
	{
		AActor* Target = FindRandomObstacleTarget(World);
		if (!Target)
		{
			return nullptr;
		}

		AImposterScaleGizmo* Actor = World->SpawnActor<AImposterScaleGizmo>();
		if (Actor)
		{
			Actor->Capture(Target);
		}
		return Actor;
	}
	default:
		return nullptr;
	}
}

AActor* FGimmickManager::FindRandomObstacleTarget(UWorld* World) const
{
	if (!World)
	{
		return nullptr;
	}

	TArray<AActor*> Candidates;
	for (AActor* Actor : World->GetActors())
	{
		if (Actor && Actor->IsA<AObstacleActorBase>())
		{
			Candidates.push_back(Actor);
		}
	}

	if (Candidates.empty())
	{
		return nullptr;
	}

	return Candidates[MapRandom::Index(static_cast<int32>(Candidates.size()))];
}

EGimmickType FGimmickManager::SelectRandomGimmickType() const
{
	const int32 TypeCount = static_cast<int32>(EGimmickType::Count);
	return static_cast<EGimmickType>(MapRandom::Index(TypeCount));
}
