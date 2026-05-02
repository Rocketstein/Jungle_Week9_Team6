#include "AMapManager.h"
#include "GameFramework/World.h"

void AMapManager::BeginPlay() {
	BuildTemplateLibrary();
}

void AMapManager::EndPlay() {
	AActor::EndPlay();
}

void AMapManager::Tick(float DeltaTime) {
	if (!Player || Templates.empty()) return;

	while ((int32)ActiveChunks.size() < TargetChunkCount)
		SpawnNextChunk();

	if (!ActiveChunks.empty())
	{
		AMapChunk* Front = ActiveChunks[0];
		FQuat ChunkQuat = FQuat::FromRotator(Front->GetActorRotation());
		FVector ToPlayer = Player->GetActorLocation() - Front->GetActorLocation();
		float Progress = ToPlayer.Dot(ChunkQuat.GetForwardVector());

		if (Progress > Front->GetChunkLength())
			DespawnFrontChunk();
	}
}

void AMapManager::BuildTemplateLibrary() {
	// Straight
	FMapChunkTemplate Straight;
	Straight.ChunkType = EChunkType::Straight;
	Straight.Length = 20.0f;
	Straight.ExitOffset = FVector(20.0f, 0.0f, 0.0f);
	Templates.push_back(Straight);

	// Left Turn
	FMapChunkTemplate LeftTurn;
	LeftTurn.ChunkType = EChunkType::TurnLeft;
	LeftTurn.ExitOffset = FVector(10.0f, -10.0f, 0.0f);
	LeftTurn.ExitRotation = FRotator(0.0f, 0.0f, -90.0f);  // Yaw -90 for left
	Templates.push_back(LeftTurn);

	// Right Turn
	FMapChunkTemplate RightTurn;
	RightTurn.ChunkType = EChunkType::TurnRight;
	RightTurn.ExitOffset = FVector(10.0f, 10.0f, 0.f);
	RightTurn.ExitRotation = FRotator(0.0f, 0.0f, 90.f);
	Templates.push_back(RightTurn);

	// Straight With Hole
	FMapChunkTemplate StraightWithHole;
	StraightWithHole.ChunkType = EChunkType::StraightWithHole;
	StraightWithHole.Length = 20.f;
	StraightWithHole.ExitOffset = FVector(20.f, 0.f, 0.f);
	Templates.push_back(StraightWithHole);
}

void AMapManager::SpawnNextChunk()
{
	int32 Idx = SelectNextTemplateIndex();
	const FMapChunkTemplate& T = Templates[Idx];

	FVector  SpawnLoc = ActiveChunks.empty() ? FVector(0, 0, 0) : ActiveChunks.back()->GetExitLocation();
	FRotator SpawnRot = ActiveChunks.empty() ? FRotator(0, 0, 0) : ActiveChunks.back()->GetExitRotation();

	AMapChunk* Chunk = GetWorld()->SpawnActor<AMapChunk>();
	Chunk->SetActorLocation(SpawnLoc);
	Chunk->SetActorRotation(SpawnRot);
	Chunk->InitFromTemplate(T);
	ActiveChunks.push_back(Chunk);

	bool bIsTurn = (T.ChunkType == EChunkType::TurnLeft || T.ChunkType == EChunkType::TurnRight);
	StraightRunLength = bIsTurn ? 0 : StraightRunLength + 1;
}

void AMapManager::DespawnFrontChunk() {
	if (ActiveChunks.empty()) return;
	ActiveChunks.front()->EndPlay();
	ActiveChunks.erase(ActiveChunks.begin());
}

int32 AMapManager::SelectNextTemplateIndex()
{
	TArray<int32> Candidates;
	for (int32 i = 0; i < (int32)Templates.size(); ++i)
	{
		EChunkType T = Templates[i].ChunkType;
		bool bIsTurn = (T == EChunkType::TurnLeft || T == EChunkType::TurnRight);
		if (bIsTurn && StraightRunLength < MinStraightsBetweenTurns)
			continue;
		Candidates.push_back(i);
	}
	return Candidates[rand() % Candidates.size()];
}