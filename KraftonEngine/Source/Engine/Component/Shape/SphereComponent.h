#pragma once
#include "ShapeComponent.h"

class USphereComponent : public UShapeComponent {
public:
	DECLARE_CLASS(USphereComponent, UShapeComponent)

	USphereComponent() = default;
	USphereComponent(float InRadius) : SphereRadius(InRadius) {} 
	float GetSphereRadius() const { return SphereRadius; }
	void  SetSphereRadius(float InRadius) { SphereRadius = InRadius; }

	void DrawDebugShape(UWorld* InWorld) const override;

private:
	float SphereRadius = 5.f;
};