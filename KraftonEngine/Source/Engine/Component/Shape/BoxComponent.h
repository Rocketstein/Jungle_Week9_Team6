#pragma once
#include "ShapeComponent.h"

class UBoxComponent : public UShapeComponent {
public:
	DECLARE_CLASS(UBoxComponent, UShapeComponent)
	UBoxComponent() = default;
	UBoxComponent(FVector InExtent) : BoxExtent(InExtent) {}
	FVector GetBoxExtent() const { return BoxExtent; }
	void	SetBoxExtent(FVector InExtent) { BoxExtent = InExtent; }

	void DrawDebugShape(UWorld* InWorld) override;

private:
	FVector BoxExtent;
};