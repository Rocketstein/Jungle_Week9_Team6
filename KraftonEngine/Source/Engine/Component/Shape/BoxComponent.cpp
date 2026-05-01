#include "BoxComponent.h"
#include "GameFramework/World.h"

IMPLEMENT_CLASS(UBoxComponent, UShapeComponent)

void UBoxComponent::DrawDebugShape(UWorld* InWorld) const {
	FVector Center		= GetWorldLocation();
	FVector WorldMin	= Center - BoxExtent;
	FVector WorldMax	= Center + BoxExtent;
	InWorld->GetScene().AddDebugAABB(WorldMin, WorldMax, ShapeColor);

	//auto GetCornerPoint = [&](int32 Bit) -> FVector
	//{
	//	FVector Corner = {};
	//	Corner.X = (Bit & 1) ? (WorldMax.X) : (WorldMin.X);
	//	Corner.Y = (Bit & 2) ? (WorldMax.Y) : (WorldMin.Y);
	//	Corner.Z = (Bit & 4) ? (WorldMax.Z) : (WorldMin.Z);
	//	return Corner;
	//};

	//for (int32 PointIdx = 1; PointIdx <= 8; ++PointIdx)
	//{
	//	for (int32 mask = 1; mask <= 4; mask <<= 1)
	//	{
	//		if (PointIdx & mask) continue;
	//		InWorld->GetScene().AddDebugAABB()

	//		AddLine(GetCornerPoint(PointIdx), GetCornerPoint(PointIdx | mask), Color);
	//	}
	//}
}
