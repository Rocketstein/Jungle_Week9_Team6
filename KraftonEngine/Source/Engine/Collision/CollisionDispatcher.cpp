#include "CollisionDispatcher.h"
#include "Component/Shape/BoxComponent.h"
#include "Component/Shape/SphereComponent.h"
#include "Component/Shape/CapsuleComponent.h"

static bool BoxBoxCollision(UShapeComponent* A, UShapeComponent* B) {
	if (!A || !B) return false;

}

static bool SphereSphereCollision(UShapeComponent* A, UShapeComponent* B) {
    if(!A || !B) return false;
}

static bool CapsuleCapsuleCollision(UShapeComponent*A, UShapeComponent* B) {
    if(!A || !B) return false;
} 

static bool BoxSphereCollision(UShapeComponent* A, UShapeComponent* B) {
    if(!A || !B) return false;

	if (UBoxComponent* Box = dynamic_cast<UBoxComponent*>(A)) {
		
	}
	else if (UBoxComponent* Box = dynamic_cast<UBoxComponent*>(B)) {
	
	}

	return false;
}

static bool BoxCapsuleCollision(UShapeComponent* A, UShapeComponent* B) {
    if (!A || !B) return false;

	if (UBoxComponent* Box = dynamic_cast<UBoxComponent*>(A)) {

	}
	else if (UBoxComponent* Box = dynamic_cast<UBoxComponent*>(B)) {

	}

	return false;
}

static bool SphereCapsuleCollision(UShapeComponent* A, UShapeComponent* B) {
    if (!A || !B) return false;

	if (USphereComponent* Sphere = dynamic_cast<USphereComponent*>(A)) {

	}
	else if (USphereComponent* Sphere = dynamic_cast<USphereComponent*>(B)) {

	}

	return false;
}

void FCollisionDispatcher::Init() {
	Register("UBoxComponent", "UBoxComponent", [](const UShapeComponent* A, const UShapeComponent* B) {
		return static_cast<const UBoxComponent*>(A)->IsOverlappingComponent(static_cast<const UBoxComponent*>(B), *(new FOverlapInfo()));
		});
	Register("UBoxComponent", "USphereComponent", [](const UShapeComponent* A, const UShapeComponent* B) {
		return static_cast<const UBoxComponent*>(A)->IsOverlappingComponent(static_cast<const USphereComponent*>(B), *(new FOverlapInfo()));
		});
	Register("UBoxComponent", "UCapsuleComponent", [](const UShapeComponent* A, const UShapeComponent* B) {
		return static_cast<const UBoxComponent*>(A)->IsOverlappingComponent(static_cast<const UCapsuleComponent*>(B), *(new FOverlapInfo()));
		});
	Register("USphereComponent", "USphereComponent", [](const UShapeComponent* A, const UShapeComponent* B) {
		return static_cast<const USphereComponent*>(A)->IsOverlappingComponent(static_cast<const USphereComponent*>(B), *(new FOverlapInfo()));
		});
	Register("USphereComponent", "UCapsuleComponent", [](const UShapeComponent* A, const UShapeComponent* B) {
		return static_cast<const USphereComponent*>(A)->IsOverlappingComponent(static_cast<const UCapsuleComponent*>(B), *(new FOverlapInfo()));
		});
	Register("UCapsuleComponent", "UCapsuleComponent", [](const UShapeComponent* A, const UShapeComponent* B) {
		return static_cast<const UCapsuleComponent*>(A)->IsOverlappingComponent(static_cast<const UCapsuleComponent*>(B), *(new FOverlapInfo()));
		});
}