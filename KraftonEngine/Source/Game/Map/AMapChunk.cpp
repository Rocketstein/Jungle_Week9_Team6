#include "AMapChunk.h"
#include "GameFramework/World.h"
#include "Component/StaticMeshComponent.h"

IMPLEMENT_CLASS(AMapChunk, AActor)

void AMapChunk::BeginPlay() {
	SpawnObstacle();
}

void AMapChunk::EndPlay() {
	for (auto* Obstacle : SpawnedObstacles) {
		Obstacle->EndPlay();
		Obstacle = nullptr;
	}

	SpawnedObstacles.clear();
	AActor::EndPlay();
}

void AMapChunk::InitFromTemplate(const FMapChunkTemplate& InTemplate) {
	Template  = InTemplate;
	FloorMesh = AddComponent<UStaticMeshComponent>();
	//SetRootComponent();
}

FVector AMapChunk::GetExitLocation() const
{
	FQuat WorldQuat = FQuat::FromRotator(GetActorRotation());
	return GetActorLocation() + WorldQuat.RotateVector(Template.ExitOffset);
}

FRotator AMapChunk::GetExitRotation() const
{
	FQuat WorldQuat = FQuat::FromRotator(GetActorRotation());
	FQuat ExitQuat = FQuat::FromRotator(Template.ExitRotation);
	return (WorldQuat * ExitQuat).ToRotator();
}


void AMapChunk::SpawnObstacle() {
	for (const FObstacleSlot& Slot : Template.ObstacleSlots)
	{
		if ((float)rand() / RAND_MAX > Template.ObstacleFillRate) continue;
		if (static_cast<uint8>(Slot.AllowedTypes) == 0) continue;

		AStaticMeshActor* Obs = GetWorld()->SpawnActor<AStaticMeshActor>();
		FQuat WorldQuat = FQuat::FromRotator(GetActorRotation());
		Obs->SetActorLocation(GetActorLocation() + WorldQuat.RotateVector(Slot.LocalPosition));
		SpawnedObstacles.push_back(Obs);
	}
}