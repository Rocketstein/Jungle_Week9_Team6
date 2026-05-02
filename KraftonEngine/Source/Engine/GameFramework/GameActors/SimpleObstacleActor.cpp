#include "SimpleObstacleActor.h"
#include "Component/StaticMeshComponent.h"
#include "Component/Shape/BoxComponent.h"
#include "Engine/Runtime/Engine.h"
#include "Materials/MaterialManager.h"
#include "Resource/ResourceManager.h"

IMPLEMENT_CLASS(ASimpleObstacleActor, AObstacleActorBase)

void ASimpleObstacleActor::InitDefaultComponents(const FString& UStaticMeshFileName) {
	StaticMeshComponent = AddComponent<UStaticMeshComponent>();
	StaticMeshComponent->SetCanDeleteFromDetails(false);
	SetRootComponent(StaticMeshComponent);
	UBoxComponent* CollisionBox = AddComponent<UBoxComponent>();

	if (!UStaticMeshFileName.empty() && UStaticMeshFileName != "None")
	{
		ID3D11Device* Device = GEngine->GetRenderer().GetFD3DDevice().GetDevice();
		UStaticMesh* Asset = FObjManager::LoadObjStaticMesh(UStaticMeshFileName, Device);
		StaticMeshComponent->SetStaticMesh(Asset);

		if (Asset && IsBasicShapeAssetPath(UStaticMeshFileName))
		{
			const FString DefaultShapeMaterialPath = FResourceManager::Get().ResolvePath(FName("Default.Material.BasicShape"));
			if (UMaterial* DefaultShapeMaterial = FMaterialManager::Get().GetOrCreateMaterial(DefaultShapeMaterialPath))
			{
				int32 MaterialCount = static_cast<int32>(Asset->GetStaticMaterials().size());
				if (MaterialCount == 0 && Asset->GetStaticMeshAsset() &&
					(!Asset->GetStaticMeshAsset()->Sections.empty() || !Asset->GetStaticMeshAsset()->Indices.empty()))
				{
					MaterialCount = 1;
				}
				for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
				{
					StaticMeshComponent->SetMaterial(MaterialIndex, DefaultShapeMaterial);
				}
			}
		}
	}
	else
	{
		StaticMeshComponent->SetStaticMesh(nullptr);
	}
}
