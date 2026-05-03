#include "AMapChunk.h"
#include "GameFramework/World.h"
#include "Component/StaticMeshComponent.h"
#include "Engine/Runtime/Engine.h"
#include "Game/GameActors/Obstacle/SimpleObstacleActor.h"
#include "Game/GameActors/Obstacle/VerticalWireActor.h"
#include "Game/GameActors/Obstacle/WireballActor.h"
#include "Materials/MaterialManager.h"
#include "Resource/ResourceManager.h"

IMPLEMENT_CLASS(AMapChunk, AActor)

void AMapChunk::BeginPlay() {
	AActor::BeginPlay();
}

void AMapChunk::EndPlay() {
	for (auto* Obstacle : SpawnedObstacles) {
		if (Obstacle && Obstacle->GetWorld() && Obstacle->GetWorld()->HasBegunPlay()) {
			Obstacle->GetWorld()->DestroyActor(Obstacle);
		}
	}

	SpawnedObstacles.clear();
	AActor::EndPlay();
}

void AMapChunk::InitFromTemplate(const FMapChunkTemplate& InTemplate, float InObstacleFillRate) {
	Template         = InTemplate;
	ObstacleFillRate = InObstacleFillRate;
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

static FString GetMeshPath(const char* MeshKey)
{
	if (const FMeshResource* MeshResource = FResourceManager::Get().FindMesh(FName(MeshKey)))
	{
		return MeshResource->Path;
	}

	return "";
}

static void ApplyBasicShapeMaterial(UStaticMeshComponent* MeshComponent, UStaticMesh* Mesh)
{
	if (!MeshComponent || !Mesh)
	{
		return;
	}

	const FString MaterialPath = FResourceManager::Get().ResolvePath(FName("Default.Material.BasicShape"));
	UMaterial* Material = FMaterialManager::Get().GetOrCreateMaterial(MaterialPath);
	if (!Material)
	{
		return;
	}

	int32 MaterialCount = static_cast<int32>(Mesh->GetStaticMaterials().size());
	if (MaterialCount == 0 && Mesh->GetStaticMeshAsset() &&
		(!Mesh->GetStaticMeshAsset()->Sections.empty() || !Mesh->GetStaticMeshAsset()->Indices.empty()))
	{
		MaterialCount = 1;
	}

	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		MeshComponent->SetMaterial(MaterialIndex, Material);
	}
}

static void ApplyCubeMesh(UStaticMeshComponent* MeshComponent)
{
	if (!MeshComponent || !GEngine)
	{
		return;
	}

	const FString MeshPath = GetMeshPath("Default.Mesh.BasicShape.Cube");
	if (MeshPath.empty())
	{
		return;
	}

	ID3D11Device* Device = GEngine->GetRenderer().GetFD3DDevice().GetDevice();
	UStaticMesh* Mesh = FObjManager::LoadObjStaticMesh(MeshPath, Device);
	MeshComponent->SetStaticMesh(Mesh);
	ApplyBasicShapeMaterial(MeshComponent, Mesh);
}

void AMapChunk::SpawnObstacle()
{
	using enum EObstacleDecision;
	FQuat WorldQuat = FQuat::FromRotator(GetActorRotation());

	for (const FDecisionSlot& DecisionSlot : Template.ObstacleSlotDecisions) {
		if ((float)rand() / RAND_MAX > ObstacleFillRate) continue;
		EObstacleDecision Decision = DecisionSlot.AllowedDecisions[rand() % DecisionSlot.AllowedDecisions.size()];

		float LaneY[3] = { -Template.Width / 1.5, 0, Template.Width / 1.5f };

		switch (Decision) {
		case (SingleBarrierLeft):
		{
			AObstacleActorBase* Obs = GetWorld()->SpawnActor<AVerticalWireActor>();
			FVector ObsPos = FVector(DecisionSlot.X, LaneY[0], 0);
			Obs->InitDefaultComponents("");
			Obs->SetActorLocation(GetActorLocation() + WorldQuat.RotateVector(ObsPos));
			if (Obs->HasActorBegunPlay())
				Obs->BeginPlay();
			SpawnedObstacles.push_back(Obs);
			break;
		}
		case (SingleBarrierMiddle):
		{
			AObstacleActorBase* Obs = GetWorld()->SpawnActor<AVerticalWireActor>();
			FVector ObsPos = FVector(DecisionSlot.X, LaneY[1], 0);
			Obs->InitDefaultComponents("");
			Obs->SetActorLocation(GetActorLocation() + WorldQuat.RotateVector(ObsPos));
			if (Obs->HasActorBegunPlay())
				Obs->BeginPlay();
			SpawnedObstacles.push_back(Obs);
			break;
		}
		case (SingleBarrierRight):
		{
			AObstacleActorBase* Obs = GetWorld()->SpawnActor<AVerticalWireActor>();
			FVector ObsPos = FVector(DecisionSlot.X, LaneY[2], 0);
			Obs->InitDefaultComponents("");
			Obs->SetActorLocation(GetActorLocation() + WorldQuat.RotateVector(ObsPos));
			if (Obs->HasActorBegunPlay())
				Obs->BeginPlay();
			SpawnedObstacles.push_back(Obs);
			break;
		}
		case (DoubleBarrierLeft):
		{
			AObstacleActorBase* Obs0 = GetWorld()->SpawnActor<AVerticalWireActor>();
			FVector ObsPos0 = FVector(DecisionSlot.X, LaneY[0], 0);
			Obs0->InitDefaultComponents("");
			Obs0->SetActorLocation(GetActorLocation() + WorldQuat.RotateVector(ObsPos0));
			if (Obs0->HasActorBegunPlay())
				Obs0->BeginPlay();
			SpawnedObstacles.push_back(Obs0);

			AObstacleActorBase* Obs1 = GetWorld()->SpawnActor<AVerticalWireActor>();
			FVector ObsPos1 = FVector(DecisionSlot.X, LaneY[1], 1);
			Obs1->InitDefaultComponents("");
			Obs1->SetActorLocation(GetActorLocation() + WorldQuat.RotateVector(ObsPos1));
			if (Obs1->HasActorBegunPlay())
				Obs1->BeginPlay();
			SpawnedObstacles.push_back(Obs1);
			break;
		}
		case (DoubleBarrierRight):
		{
			AObstacleActorBase* Obs0 = GetWorld()->SpawnActor<AVerticalWireActor>();
			FVector ObsPos0 = FVector(DecisionSlot.X, LaneY[2], 0);
			Obs0->InitDefaultComponents("");
			Obs0->SetActorLocation(GetActorLocation() + WorldQuat.RotateVector(ObsPos0));
			if (Obs0->HasActorBegunPlay())
				Obs0->BeginPlay();
			SpawnedObstacles.push_back(Obs0);

			AObstacleActorBase* Obs1 = GetWorld()->SpawnActor<AVerticalWireActor>();
			FVector ObsPos1 = FVector(DecisionSlot.X, LaneY[1], 1);
			Obs1->InitDefaultComponents("");
			Obs1->SetActorLocation(GetActorLocation() + WorldQuat.RotateVector(ObsPos1));
			if (Obs1->HasActorBegunPlay())
				Obs1->BeginPlay();
			SpawnedObstacles.push_back(Obs1);
			break;
		}
		case (MustJump):
		{
			//AObstacleActorBase* Obs = GetWorld()->SpawnActor<AVerticalWireActor>();
			//FVector ObsPos = FVector(DecisionSlot.X, LaneY[2], 0);
			//Obs->InitDefaultComponents("");
			//Obs->SetActorLocation(GetActorLocation() + WorldQuat.RotateVector(ObsPos));
			//if (Obs->HasActorBegunPlay())
			//	Obs->BeginPlay();
			//SpawnedObstacles.push_back(Obs);
			break;
		}
		case (MustSlide):
		{
			//AObstacleActorBase* Obs = GetWorld()->SpawnActor<AVerticalWireActor>();
			//FVector ObsPos = FVector(DecisionSlot.X, LaneY[2], 0);
			//Obs->InitDefaultComponents("");
			//Obs->SetActorLocation(GetActorLocation() + WorldQuat.RotateVector(ObsPos));
			//if (Obs->HasActorBegunPlay())
			//	Obs->BeginPlay();
			//SpawnedObstacles.push_back(Obs);
			break;
		}
		default: {
			break;
		}
		}
	}
}

void AMapChunk::BuildFloor() {
	for (UStaticMeshComponent* FloorMesh : FloorMeshes)
	{
		RemoveComponent(FloorMesh);
	}
	FloorMeshes.clear();

	for (const FFloorBlock& BlockInfo : Template.FloorBlockInfos) {
		UStaticMeshComponent* Block = AddComponent<UStaticMeshComponent>();
		if (GetRootComponent())
		{
			Block->AttachToComponent(GetRootComponent());
		}

		ApplyCubeMesh(Block);
		FVector BlockPos = BlockInfo.LocalPosition;
		Block->SetRelativeLocation(FVector(BlockPos.X, BlockPos.Y, BlockPos.Z));
		Block->SetRelativeRotation(BlockInfo.LocalRotation);
		Block->SetRelativeScale(BlockInfo.Scale);
		FloorMeshes.push_back(Block);
	}

	// AddComponent does not update the spatial partition, so re-insert the actor
	// so the new floor mesh components become visible to frustum culling.
	if (UWorld* World = GetWorld())
	{
		World->InsertActorToOctree(this);
	}
}
