#pragma once
#include "ObstacleActorBase.h"

// Basic, static obstacle type
class ASimpleObstacleActor : public AObstacleActorBase {
public:
	DECLARE_CLASS(ASimpleObstacleActor, AObstacleActorBase)

private:
	void OnPlayerCollision() override {}

private:
	//UShapeComponent* CollisionShape = nullptr;
};