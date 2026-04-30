#pragma once
#include "ShapeComponent.h"

class USphereComponent : public UShapeComponent {
public:
	DECLARE_CLASS(USphereComponent, UShapeComponent)

	USphereComponent(float InRadius) : SphereRadius(InRadius) {} 
	float GetSphereRadius() const { return SphereRadius; }
	void  SetSphereRadius(float InRadius) { SphereRadius = InRadius; }

private:
	float SphereRadius;
};