#pragma once
#include "ShapeComponent.h"

class UCapsuleComponent : public UShapeComponent {
public:
	DECLARE_CLASS(UCapsuleComponent, UShapeComponent)

	UCapsuleComponent(float InHalfHeight, float InRadius) : CapsuleHalfHeight(InHalfHeight), CapsuleRadius(InRadius) {}
	float GetCapsuleHalfHeight() const { return CapsuleHalfHeight; }
	float GetCapsuleRadius() const { return CapsuleRadius; }
	void  SetCapsuleHalfHeight(float InHalfHeight) { CapsuleHalfHeight = InHalfHeight; }
	void  SetCapsuleRadius(float InRadius) { CapsuleRadius = InRadius; }

private:
	float CapsuleHalfHeight;
	float CapsuleRadius;
};