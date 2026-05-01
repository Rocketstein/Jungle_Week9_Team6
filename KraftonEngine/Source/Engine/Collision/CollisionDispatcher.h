#pragma once
#include "Core/Singleton.h"
#include "Collision/OverlapInfo.h"
#include "Component/Shape/ShapeComponent.h"

class FCollisionDispatcher : public TSingleton<FCollisionDispatcher> {
	friend class TSingleton<FCollisionDispatcher>;
	using ShapeCollisionFunc = bool(*)(const UShapeComponent*, const UShapeComponent*);

public:
	void Init();

	void Register(FString Shape0, FString Shape1, ShapeCollisionFunc Func) {
		CollisionMatrix[std::pair<FString, FString>(Shape0, Shape1)] = Func;
		CollisionMatrix[std::pair<FString, FString>(Shape1, Shape0)] = Func;
	}

	bool CheckCollision(const UShapeComponent* ShapeA, const UShapeComponent* ShapeB) const {
		FString TypeA = ShapeA->GetClass()->GetName();
		FString TypeB = ShapeB->GetClass()->GetName();
		auto it = CollisionMatrix.find(std::pair<FString, FString>(TypeA, TypeB));
		if (it != CollisionMatrix.end()) {
			return it->second(ShapeA, ShapeB, InInfo);
		}
		return false;	// No registered collision function for this pair
	}

private:
	TMap<std::pair<FString, FString>, ShapeCollisionFunc> CollisionMatrix;
};

