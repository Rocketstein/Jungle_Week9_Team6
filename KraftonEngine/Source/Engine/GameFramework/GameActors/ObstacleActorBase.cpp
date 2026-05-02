#include "ObstacleActorBase.h"  
#include "Component/Shape/BoxComponent.h"  
#include "Component/Shape/SphereComponent.h"  
#include "Component/Shape/CapsuleComponent.h"

#include "Core/Log.h"

DEFINE_CLASS(AObstacleActorBase, AStaticMeshActor)

void AObstacleActorBase::BeginPlay() {
	Super::BeginPlay();
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
	UE_LOG("Listening to a Hit Event, UUID: " + this->GetUUID());
	if (!Other.HitComponent) return;
	
}

void AObstacleActorBase::OnOverlap(const FComponentOverlapEvent& Other) {
	UE_LOG("Listening to an Overlap Event, UUID: " + this->GetUUID());
	if (!Other.OtherComponent) return;
}