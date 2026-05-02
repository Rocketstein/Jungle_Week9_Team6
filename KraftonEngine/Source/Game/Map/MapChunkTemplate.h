#pragma once
#include "Game/GameActors/Obstacle/ObstacleActorBase.h"
#include "Core/CoreTypes.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"

enum class EChunkType {
	Straight,
	TurnLeft,
	TurnRight,
	TwoWay,
	StraightWithHole,
};

struct FObstacleSlot
{
	FVector			LocalPosition;  // where in the chunk this obstacle can appear
	EObstacleType	AllowedTypes;   // bitmask - which obstacle types fit here
};

 /*
 At spawn, AMapChunk::BeginPlay() iterates ObstacleSlots, 
 rolls against ObstacleFillRate for each, and calls World->SpawnActor<>() for the winners. 
 This gives runtime variety while the template guarantees at least one lane always stays open
 */
struct FMapChunkTemplate {
	EChunkType				ChunkType = EChunkType::Straight;
	float					Length;
	FVector					ExitOffset;							// local-space offset from entry to next chunk's origin
	FRotator				ExitRotation;						// e.g. (0, 0, -90) for a left turn around Z
	TArray<FObstacleSlot>	ObstacleSlots;						// available slots, defined per template
	float					ObstacleFillRate;					// 0.0–1.0, tunable per chunk type
};