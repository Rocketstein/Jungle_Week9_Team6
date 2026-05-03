#include "AMapManager.h"
#include "GameFramework/World.h"

IMPLEMENT_CLASS(AMapManager, AActor)

AMapManager::AMapManager()
{
	bTickInEditor = true;
	BuildTemplateLibrary();
}

void AMapManager::BeginPlay() {
	AActor::BeginPlay();
	BuildTemplateLibrary();
}

void AMapManager::EndPlay() {
	AActor::EndPlay();
}

void AMapManager::Tick(float DeltaTime) {
	//if (!Player) return; 
	if (Templates.empty()) return;

	while ((int32)ActiveChunks.size() < TargetChunkCount)
		SpawnNextChunk();

	if (!ActiveChunks.empty())
	{
		AMapChunk* Front = ActiveChunks[0];
		FQuat ExitQuat = FQuat::FromRotator(Front->GetExitRotation());
		//FVector ToPlayer = Player->GetActorLocation() - Front->GetExitLocation();
		//float ExitProgress = ToPlayer.Dot(ExitQuat.GetForwardVector());

		//if (ExitProgress > 0.0f)
		//	DespawnFrontChunk();
	}
}

void AMapManager::BuildTemplateLibrary() {
	constexpr float ChunkWidth  = 6.f;	// Recommended multiple of 3
	constexpr float ChunkLength = 20.f; // Recommended multiple of 2
	constexpr float TurnLength = 10.f;

	Templates.clear();

	// TODO: Walls
	//-----------------------------------------------------------------
	// Straight
	//-----------------------------------------------------------------
	FMapChunkTemplate Straight;
	Straight.ChunkType = EChunkType::Straight;
	Straight.Length = ChunkLength;
	Straight.ExitOffset = FVector(ChunkLength, 0.0f, 0.0f);

	// Straight floor infos
	FFloorBlock StraightFloor = {};
	StraightFloor.LocalPosition = FVector(ChunkLength * 0.5f, 0, 0);
	StraightFloor.LocalRotation = FRotator(0, 0, 0);
	StraightFloor.Scale			= FVector(ChunkLength * 0.5, ChunkWidth, 1);
	Straight.FloorBlockInfos.push_back(StraightFloor);

	// Straight wall infos
	//FFloorBlock StraightWallLeft = {};
	//StraightWallLeft.LocalPosition = FVector(0, -1, 0);
	//StraightWallLeft.LocalRotation = FRotator(0, 0, 0);
	//StraightWallLeft.Scale		   = FVector(ChunkLength, 1, Wallheight);
	//Straight.FloorBlockInfos.push_back(StraightWallLeft);

	//FFloorBlock StraightWallRight = {};
	//StraightWallRight.LocalPosition = FVector(0, ChunkWidth + 1, 0);
	//StraightWallRight.LocalRotation = FRotator(0, 0, 0);
	//StraightWallRight.Scale			= FVector(ChunkLength, 1, Wallheight);
	//Straight.FloorBlockInfos.push_back(StraightWallRight);
	Templates.push_back(Straight);

	//-----------------------------------------------------------------
	// Left Turn
	//-----------------------------------------------------------------
	//FMapChunkTemplate LeftTurn;
	//LeftTurn.ChunkType = EChunkType::TurnLeft;
	//LeftTurn.Length = ChunkLength;
	//LeftTurn.ExitOffset = FVector(TurnLength, -TurnLength, 0.0f);
	//LeftTurn.ExitRotation = FRotator(0.0f, -90.0f, 0.0f);  // Yaw -90 for left

	//// Left Turn floor
	//FFloorBlock LeftTurnFloorStraight = {};
	//LeftTurnFloorStraight.LocalPosition = FVector(TurnLength * 0.5f, 0, 0);
	//LeftTurnFloorStraight.LocalRotation = FRotator(0, 0, 0);
	//LeftTurnFloorStraight.Scale			= FVector(TurnLength, ChunkWidth, 1);
	//LeftTurn.FloorBlockInfos.push_back(LeftTurnFloorStraight);

	//FFloorBlock LeftTurnFloorCorner = {};
	//LeftTurnFloorCorner.LocalPosition = FVector(TurnLength + ChunkWidth * 0.25f, -ChunkWidth * 0.25f, 0);
	//LeftTurnFloorCorner.LocalRotation = FRotator(0, 0, 0);
	//LeftTurnFloorCorner.Scale		 = FVector(ChunkWidth * 0.5f, ChunkWidth * 0.5f, 1);
	//LeftTurn.FloorBlockInfos.push_back(LeftTurnFloorCorner);

	//FFloorBlock LeftTurnFloorExit = {};
	//LeftTurnFloorExit.LocalPosition = FVector(TurnLength, -(TurnLength + ChunkWidth * 0.5f) * 0.5f, 0);
	//LeftTurnFloorExit.LocalRotation = FRotator(0, -90, 0);
	//LeftTurnFloorExit.Scale			= FVector(TurnLength - ChunkWidth * 0.5f, ChunkWidth, 1);
	//LeftTurn.FloorBlockInfos.push_back(LeftTurnFloorExit);

	// Left Turn Wall
	//FFloorBlock LeftTurnLeftWallStraight = {};
	//LeftTurnLeftWallStraight.LocalPosition = FVector(0, -1, 0);
	//LeftTurnLeftWallStraight.LocalRotation = FRotator(0, 0, 0);
	//LeftTurnLeftWallStraight.Scale		   = FVector(ChunkLength / 2 - ChunkWidth, 1, Wallheight);
	//LeftTurn.FloorBlockInfos.push_back(LeftTurnLeftWallStraight);

	//FFloorBlock LeftTurnRightWallStraight = {};
	//LeftTurnRightWallStraight.LocalPosition = FVector(0, ChunkWidth + 1, Wallheight);
	//LeftTurnRightWallStraight.LocalRotation = FRotator(0, 0, 0);
	//LeftTurnRightWallStraight.Scale			= FVector(ChunkLength / 2 , 1, Wallheight);
	//LeftTurn.FloorBlockInfos.push_back(LeftTurnRightWallStraight);

	//FFloorBlock LeftTurnLeftWallTurn = {};
	//LeftTurnLeftWallTurn.LocalPosition = FVector(ChunkLength / 2 - ChunkWidth, -1 , 0);
	//LeftTurnLeftWallTurn.LocalRotation = FRotator(0, 0, -90);
	//LeftTurnLeftWallTurn.Scale		   = FVector(ChunkLength / 2 - ChunkWidth, 1, Wallheight);
	//LeftTurn.FloorBlockInfos.push_back(LeftTurnLeftWallTurn);

	//FFloorBlock LeftTurnRightWallTurn = {};
	//LeftTurnRightWallTurn.LocalPosition = FVector(ChunkLength / 2, ChunkWidth + 1, 0);
	//LeftTurnRightWallTurn.LocalRotation = FRotator(0, 0, -90);
	//LeftTurnRightWallTurn.Scale			= FVector(ChunkLength / 2, 1 , Wallheight);
	//LeftTurn.FloorBlockInfos.push_back(LeftTurnRightWallTurn);

	//Templates.push_back(LeftTurn);

	//-----------------------------------------------------------------
	// Right Turn
	//-----------------------------------------------------------------
	//FMapChunkTemplate RightTurn;
	//RightTurn.ChunkType = EChunkType::TurnRight;
	//RightTurn.Length = ChunkLength;
	//RightTurn.ExitOffset = FVector(TurnLength, TurnLength, 0.f);
	//RightTurn.ExitRotation = FRotator(0.0f, 90.0f, 0.f);

	//// Right Turn Floor
	//FFloorBlock RightTurnFloorStraight = {};
	//RightTurnFloorStraight.LocalPosition = FVector(TurnLength * 0.5f, 0, 0);
	//RightTurnFloorStraight.LocalRotation = FRotator(0, 0, 0);
	//RightTurnFloorStraight.Scale		 = FVector(TurnLength, ChunkWidth, 1);
	//RightTurn.FloorBlockInfos.push_back(RightTurnFloorStraight);

	//FFloorBlock RightTurnFloorCorner = {};
	//RightTurnFloorCorner.LocalPosition = FVector(TurnLength + ChunkWidth * 0.25f, ChunkWidth * 0.25f, 0);
	//RightTurnFloorCorner.LocalRotation = FRotator(0, 0, 0);
	//RightTurnFloorCorner.Scale		 = FVector(ChunkWidth * 0.5f, ChunkWidth * 0.5f, 1);
	//RightTurn.FloorBlockInfos.push_back(RightTurnFloorCorner);

	//FFloorBlock RightTurnFloorExit = {};
	//RightTurnFloorExit.LocalPosition = FVector(TurnLength, (TurnLength + ChunkWidth * 0.5f) * 0.5f, 0);
	//RightTurnFloorExit.LocalRotation = FRotator(0, 90, 0);
	//RightTurnFloorExit.Scale		 = FVector(TurnLength - ChunkWidth * 0.5f, ChunkWidth, 1);
	//RightTurn.FloorBlockInfos.push_back(RightTurnFloorExit);

	// Right Turn Walls
	//FFloorBlock RighTurnLeftWallStraight;
	//RighTurnLeftWallStraight.LocalPosition = FVector(0, -1, 0);
	//RighTurnLeftWallStraight.LocalRotation = FRotator(0, 0, 0);
	//RighTurnLeftWallStraight.Scale		   = FVector(ChunkLength / 2, 1, Wallheight);
	//RightTurn.FloorBlockInfos.push_back(RighTurnLeftWallStraight);

	//FFloorBlock RightTurnRightWallStraight;
	//RightTurnRightWallStraight.LocalPosition = FVector(0, ChunkWidth + 1, 0);
	//RightTurnRightWallStraight.LocalRotation = FRotator(0, 0, 0);
	//RightTurnRightWallStraight.Scale		 = FVector(ChunkLength / 2 - ChunkWidth, 1, Wallheight);
	//RightTurn.FloorBlockInfos.push_back(RightTurnRightWallStraight);

	//FFloorBlock RightTurnLeftWallTurn;
	//RightTurnLeftWallTurn.LocalPosition = FVector(ChunkLength / 2 + ChunkWidth, -1, 0);
	//RightTurnLeftWallTurn.LocalRotation = FRotator(0, 0, 90);
	//RightTurnLeftWallTurn.Scale			= FVector(ChunkLength / 2, 1, Wallheight);
	//RightTurn.FloorBlockInfos.push_back(RightTurnLeftWallTurn);

	//FFloorBlock RightTurnRIghtWallTurn;
	//RightTurnRIghtWallTurn.LocalPosition = FVector(ChunkLength / 2, ChunkWidth + 1, 0);
	//RightTurnRIghtWallTurn.LocalRotation = FRotator(0, 0, 90);
	//RightTurnRIghtWallTurn.Scale		 = FVector(ChunkLength / 2 - ChunkWidth, 1, Wallheight);
	//RightTurn.FloorBlockInfos.push_back(RightTurnRIghtWallTurn);

	//Templates.push_back(RightTurn);

	//-----------------------------------------------------------------
	// Straight With Hole
	//-----------------------------------------------------------------
	FMapChunkTemplate StraightWithHole;
	StraightWithHole.ChunkType = EChunkType::StraightWithHole;
	StraightWithHole.Length = ChunkLength;
	StraightWithHole.ExitOffset = FVector(ChunkLength, 0.f, 0.f);

	// Straight With Hole Floor
	FFloorBlock StraightWithHoleFloor1 = {};
	StraightWithHoleFloor1.LocalPosition = FVector(ChunkLength / 8.f, 0, 0);
	StraightWithHoleFloor1.LocalRotation = FRotator(0, 0, 0);
	StraightWithHoleFloor1.Scale		 = FVector(ChunkLength / 8, ChunkWidth, 1);
	StraightWithHole.FloorBlockInfos.push_back(StraightWithHoleFloor1);

	FFloorBlock StraightWithHoleFloor2 = {};
	StraightWithHoleFloor2.LocalPosition = FVector(ChunkLength * 0.75f, 0, 0);
	StraightWithHoleFloor2.LocalRotation = FRotator(0, 0, 0);
	StraightWithHoleFloor2.Scale		 = FVector(ChunkLength / 4, ChunkWidth, 1);
	StraightWithHole.FloorBlockInfos.push_back(StraightWithHoleFloor2);

	// Straight With Hole Walls
	//FFloorBlock StraightWithHoleWallLeft = {};
	//StraightWithHoleWallLeft.LocalPosition = FVector(0, -1, 0);
	//StraightWithHoleWallLeft.LocalRotation = FRotator(0, 0, 0);
	//StraightWithHoleWallLeft.Scale = FVector(ChunkLength, 1, Wallheight);
	//StraightWithHole.FloorBlockInfos.push_back(StraightWithHoleWallLeft);

	//FFloorBlock StraightWithHoleWallRight = {};
	//StraightWithHoleWallRight.LocalPosition = FVector(0, ChunkWidth + 1, 0);
	//StraightWithHoleWallRight.LocalRotation = FRotator(0, 0, 0);
	//StraightWithHoleWallRight.Scale = FVector(ChunkLength, 1, Wallheight);
	//StraightWithHole.FloorBlockInfos.push_back(StraightWithHoleWallRight);

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

	//bool bIsTurn = (T.ChunkType == EChunkType::TurnLeft || T.ChunkType == EChunkType::TurnRight);
	//StraightRunLength = bIsTurn ? 0 : StraightRunLength + 1;
}

void AMapManager::DespawnFrontChunk() {
	if (ActiveChunks.empty()) return;
	GetWorld()->DestroyActor(ActiveChunks.front());
	ActiveChunks.erase(ActiveChunks.begin());
}

int32 AMapManager::SelectNextTemplateIndex()
{
	TArray<int32> Candidates;
	for (int32 i = 0; i < (int32)Templates.size(); ++i)
	{
		EChunkType T = Templates[i].ChunkType;
		//bool bIsTurn = (T == EChunkType::TurnLeft || T == EChunkType::TurnRight);
		//if (bIsTurn && StraightRunLength < MinStraightsBetweenTurns)
		//	continue;
		Candidates.push_back(i);
	}
	return Candidates[rand() % Candidates.size()];
}
