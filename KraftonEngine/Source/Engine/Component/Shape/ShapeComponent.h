#pragma once
#include "Component/PrimitiveComponent.h"

class UShapeComponent : public UPrimitiveComponent {
public:
	DECLARE_CLASS(UShapeComponent, UPrimitiveComponent)
	virtual ~UShapeComponent() = default;

public:
	FColor	ShapeColor;
	bool	bDrawOnlyIfSelected = false;

protected:

protected:

};