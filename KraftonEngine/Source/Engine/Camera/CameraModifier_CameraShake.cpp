#include "Camera/CameraModifier_CameraShake.h"

#include "Camera/CameraShake.h"
#include "Camera/CameraTypes.h"
#include "Object/Object.h"
#include "Object/ObjectFactory.h"
#include "Object/UClass.h"

IMPLEMENT_CLASS(UCameraModifier_CameraShake, UCameraModifier)

UCameraShakeBase* UCameraModifier_CameraShake::AddCameraShake(UClass* ShakeClass, float Scale)
{
	if (!ShakeClass) return nullptr;
	if (!ShakeClass->IsA(UCameraShakeBase::StaticClass())) return nullptr;
	return AddCameraShakeByName(ShakeClass->GetName(), Scale);
}

UCameraShakeBase* UCameraModifier_CameraShake::AddCameraShakeByName(const char* ShakeClassName, float Scale)
{
	if (!ShakeClassName) return nullptr;

	UObject* NewObj = FObjectFactory::Get().Create(ShakeClassName, this);
	UCameraShakeBase* NewShake = NewObj ? Cast<UCameraShakeBase>(NewObj) : nullptr;
	if (!NewShake)
	{
		if (NewObj) UObjectManager::Get().DestroyObject(NewObj);
		return nullptr;
	}

	NewShake->StartShake(Scale);
	ActiveShakes.push_back(NewShake);
	return NewShake;
}

void UCameraModifier_CameraShake::RemoveCameraShake(UCameraShakeBase* ShakeInstance, bool bImmediate)
{
	if (!ShakeInstance) return;
	auto It = std::find(ActiveShakes.begin(), ActiveShakes.end(), ShakeInstance);
	if (It == ActiveShakes.end()) return;

	ShakeInstance->StopShake(bImmediate);
	if (bImmediate)
	{
		UObjectManager::Get().DestroyObject(ShakeInstance);
		ActiveShakes.erase(It);
	}
}

void UCameraModifier_CameraShake::RemoveAllCameraShakes(bool bImmediate)
{
	if (bImmediate)
	{
		for (UCameraShakeBase* Shake : ActiveShakes)
		{
			if (Shake)
			{
				Shake->StopShake(true);
				UObjectManager::Get().DestroyObject(Shake);
			}
		}
		ActiveShakes.clear();
	}
	else
	{
		for (UCameraShakeBase* Shake : ActiveShakes)
		{
			if (Shake) Shake->StopShake(false);
		}
	}
}

bool UCameraModifier_CameraShake::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutView)
{
	if (bDisabled) return false;

	FVector LocAccum = FVector::ZeroVector;
	FRotator RotAccum = FRotator::ZeroRotator;

	for (int32 i = static_cast<int32>(ActiveShakes.size()) - 1; i >= 0; --i)
	{
		UCameraShakeBase* Shake = ActiveShakes[i];
		if (!Shake)
		{
			ActiveShakes.erase(ActiveShakes.begin() + i);
			continue;
		}

		FVector LocOffset = FVector::ZeroVector;
		FRotator RotOffset = FRotator::ZeroRotator;
		Shake->UpdateAndApplyCameraShake(DeltaTime, LocOffset, RotOffset);

		LocAccum += LocOffset;
		RotAccum.Pitch += RotOffset.Pitch;
		RotAccum.Yaw += RotOffset.Yaw;
		RotAccum.Roll += RotOffset.Roll;

		if (Shake->IsFinished())
		{
			UObjectManager::Get().DestroyObject(Shake);
			ActiveShakes.erase(ActiveShakes.begin() + i);
		}
	}

	InOutView.Location += LocAccum;
	InOutView.Rotation.Pitch += RotAccum.Pitch;
	InOutView.Rotation.Yaw += RotAccum.Yaw;
	InOutView.Rotation.Roll += RotAccum.Roll;

	return false; // 후속 modifier 계속 진행
}

void UCameraModifier_CameraShake::OnRemoved()
{
	RemoveAllCameraShakes(true);
}
