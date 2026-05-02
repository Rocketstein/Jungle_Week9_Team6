#include "ObstacleActorBase.h"  
#include "Component/Shape/BoxComponent.h"  
#include "Component/Shape/SphereComponent.h"  
#include "Component/Shape/CapsuleComponent.h"

DEFINE_CLASS(AObstacleActorBase, AStaticMeshActor)

void AObstacleActorBase::BeginPlay() {
	for (UActorComponent* Comp : OwnedComponents)
	{
		if (UShapeComponent* Shape = Cast<UShapeComponent>(Comp))
		{
			Shape->OnComponentBeginOverlap.AddDynamic(this, &AObstacleActorBase::OnOverlap);
			Shape->OnComponentHit.AddDynamic(this, &AObstacleActorBase::OnHit);
		}
	}
}

void AObstacleActorBase::OnHit(const FComponentHitEvent& Other) {
	if (!Other.HitComponent) return;
}

void AObstacleActorBase::OnOverlap(const FComponentOverlapEvent& Other) {
	if (!Other.OtherComponent) return;
}