#pragma once
#include "Component/PrimitiveComponent.h"

class UShapeComponent : public UPrimitiveComponent {
public:
	DECLARE_CLASS(UShapeComponent, UPrimitiveComponent)
	virtual ~UShapeComponent() = default;
	virtual void DrawDebugShape(UWorld* InWorld) = 0;

public:
	FColor	ShapeColor;
	bool	bDrawOnlyIfSelected = false;

protected:

protected:

};