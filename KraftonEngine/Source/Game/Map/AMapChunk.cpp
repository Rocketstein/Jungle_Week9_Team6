#include "AMapChunk.h"
#include "GameFramework/World.h"
#include "Component/StaticMeshComponent.h"
#include "Game/GameActors/Obstacle/SimpleObstacleActor.h"
#include "Game/GameActors/Obstacle/VerticalWireActor.h"
#include "Game/GameActors/Obstacle/WireballActor.h"

IMPLEMENT_CLASS(AMapChunk, AActor)

void AMapChunk::BeginPlay() {
	
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
	SpawnObstacle();
	BuildFloor();
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


static AObstacleActorBase* SpawnObstacleOfType(UWorld* World, EObstacleType Type)
{
	switch (Type)
	{
	case EObstacleType::Barrier: return World->SpawnActor<AVerticalWireActor>();
	case EObstacleType::LowBar:
	case EObstacleType::HighBar: return World->SpawnActor<ASimpleObstacleActor>();
	case EObstacleType::Misc:    return World->SpawnActor<AWireballActor>();
	default:                     return nullptr;
	}
}

void AMapChunk::SpawnObstacle()
{
	FQuat WorldQuat = FQuat::FromRotator(GetActorRotation());

	for (const FObstacleSlot& Slot : Template.ObstacleSlots)
	{
		if ((float)rand() / RAND_MAX > Template.ObstacleFillRate) continue;

		uint8 Mask = static_cast<uint8>(Slot.AllowedTypes);
		if (Mask == 0) continue;

		TArray<EObstacleType> Candidates;
		for (uint8 Bit = 1; Bit != 0; Bit <<= 1)
			if (Mask & Bit) Candidates.push_back(static_cast<EObstacleType>(Bit));

		EObstacleType Chosen = Candidates[rand() % Candidates.size()];
		AObstacleActorBase* Obs = SpawnObstacleOfType(GetWorld(), Chosen);
		if (!Obs) continue;

		Obs->InitDefaultComponents("");
		Obs->SetActorLocation(GetActorLocation() + WorldQuat.RotateVector(Slot.LocalPosition));
		SpawnedObstacles.push_back(Obs);
	}
}

void AMapChunk::BuildFloor() {
	TArray<FFloorBlock> FloorBlockInfos = Template.FloorBlockInfos;
	for (uint16 i = 0; i < FloorBlockInfos.size(); i++) {
		const auto& BlockInfo = FloorBlockInfos[i];
		UStaticMeshComponent* Block = AddComponent<UStaticMeshComponent>();
		if (i == 0) {
			RootMesh = Block;
			SetRootComponent(Block);
		}

		Block->SetRelativeLocation(BlockInfo.LocalPosition);
		Block->SetRelativeRotation(BlockInfo.LocalRotation);
		Block->SetRelativeScale(BlockInfo.Scale);
	}
}