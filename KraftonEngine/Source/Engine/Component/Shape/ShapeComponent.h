#pragma once
#include "Component/PrimitiveComponent.h"

class UShapeComponent : public UPrimitiveComponent {
public:
	DECLARE_CLASS(UShapeComponent, UPrimitiveComponent)
	virtual ~UShapeComponent() = default;
	virtual void DrawDebugShape(UWorld* InWorld) const = 0;

public:
	FColor	ShapeColor;
	bool	bDrawOnlyIfSelected = false;

protected:
	void DrawDebugRing(FVector Center, float Radius, FVector AxisA, FVector AxisB, uint32 Segment, bool Half, UWorld* InWorld) const;

protected:

};