#include "GameFramework/PlayerCameraManager.h"

#include "Camera/CameraModifier.h"
#include "Component/CameraComponent.h"
#include "Object/Object.h"
#include "Object/ObjectFactory.h"
#include "Object/UClass.h"

IMPLEMENT_CLASS(APlayerCameraManager, AActor)

APlayerCameraManager::APlayerCameraManager()
{
	// PCM은 카메라 매 프레임 갱신을 위해 자체 tick 필요. PlayerController가 직접 호출하지만
	// Editor에서 빌트인 ticking을 막기 위해 bTickInEditor=false로 둔다.
	bNeedsTick = false;
	bTickInEditor = false;
}

void APlayerCameraManager::SetViewTarget(AActor* NewViewTarget)
{
	ViewTarget = NewViewTarget;
}

UCameraComponent* APlayerCameraManager::GetViewTargetCamera() const
{
	if (!ViewTarget) return nullptr;
	for (UActorComponent* Comp : ViewTarget->GetComponents())
	{
		if (UCameraComponent* Cam = Cast<UCameraComponent>(Comp))
		{
			return Cam;
		}
	}
	return nullptr;
}

void APlayerCameraManager::UpdateCamera(float DeltaTime)
{
	UCameraComponent* TargetCamera = GetViewTargetCamera();
	if (!TargetCamera) return;

	// 카메라 자체 TickComponent가 이미 자기 modifier 체인을 평가해 SelfAdditive*/SelfHitEffect를 캐시함.
	// PCM은 그 결과 위에 자체 modifier(글로벌 fade, 레터박스 등)를 추가 적용해
	// External layer로 카메라에 push (View matrix는 두 layer를 합산).
	const FVector BaseLoc = TargetCamera->GetWorldLocation() + TargetCamera->GetSelfAdditiveLocation();
	FRotator BaseWorldRot(TargetCamera->GetWorldRotation());
	FRotator BaseRot;
	BaseRot.Pitch = BaseWorldRot.Pitch + TargetCamera->GetSelfAdditiveRotation().Pitch;
	BaseRot.Yaw   = BaseWorldRot.Yaw   + TargetCamera->GetSelfAdditiveRotation().Yaw;
	BaseRot.Roll  = BaseWorldRot.Roll  + TargetCamera->GetSelfAdditiveRotation().Roll;

	FMinimalViewInfo View;
	View.Location = BaseLoc;
	View.Rotation = BaseRot;
	View.FOV = TargetCamera->GetFOV();
	View.NearZ = TargetCamera->GetNearPlane();
	View.FarZ = TargetCamera->GetFarPlane();
	View.OrthoWidth = TargetCamera->GetOrthoWidth();
	View.bIsOrthogonal = TargetCamera->IsOrthogonal();
	View.HitEffectIntensity = TargetCamera->GetSelfHitEffectIntensity();

	// PCM 자체 modifier 적용
	for (UCameraModifier* Mod : Modifiers)
	{
		if (!Mod || Mod->bDisabled) continue;
		if (Mod->ModifyCamera(DeltaTime, View)) break;
	}

	CachedView = View;

	// PCM이 base에 추가한 만큼만 External layer로 push (카메라 Self layer는 보존)
	const FVector ExtLoc = View.Location - BaseLoc;
	FRotator ExtRot;
	ExtRot.Pitch = View.Rotation.Pitch - BaseRot.Pitch;
	ExtRot.Yaw   = View.Rotation.Yaw   - BaseRot.Yaw;
	ExtRot.Roll  = View.Rotation.Roll  - BaseRot.Roll;
	const float ExtHit = View.HitEffectIntensity - TargetCamera->GetSelfHitEffectIntensity();

	TargetCamera->SetExternalAdditiveOffsets(ExtLoc, ExtRot, ExtHit);
}

UCameraModifier* APlayerCameraManager::AddCameraModifier(UClass* ModifierClass)
{
	if (!ModifierClass) return nullptr;
	if (!ModifierClass->IsA(UCameraModifier::StaticClass())) return nullptr;
	return AddCameraModifierByName(ModifierClass->GetName());
}

UCameraModifier* APlayerCameraManager::AddCameraModifierByName(const char* ModifierClassName)
{
	if (!ModifierClassName) return nullptr;
	UObject* NewObj = FObjectFactory::Get().Create(ModifierClassName, this);
	UCameraModifier* NewMod = NewObj ? Cast<UCameraModifier>(NewObj) : nullptr;
	if (!NewMod)
	{
		if (NewObj) UObjectManager::Get().DestroyObject(NewObj);
		return nullptr;
	}
	Modifiers.push_back(NewMod);
	NewMod->OnAdded();
	return NewMod;
}

void APlayerCameraManager::RemoveCameraModifier(UCameraModifier* Modifier)
{
	if (!Modifier) return;
	auto It = std::find(Modifiers.begin(), Modifiers.end(), Modifier);
	if (It == Modifiers.end()) return;
	Modifier->OnRemoved();
	Modifiers.erase(It);
	UObjectManager::Get().DestroyObject(Modifier);
}

UCameraShakeBase* APlayerCameraManager::PlayCameraShake(UClass* ShakeClass, float Scale)
{
	UCameraComponent* Cam = GetViewTargetCamera();
	return Cam ? Cam->StartCameraShake(ShakeClass, Scale) : nullptr;
}

UCameraShakeBase* APlayerCameraManager::PlayCameraShakeByName(const char* ShakeClassName, float Scale)
{
	UCameraComponent* Cam = GetViewTargetCamera();
	return Cam ? Cam->StartCameraShakeByName(ShakeClassName, Scale) : nullptr;
}

void APlayerCameraManager::StopCameraShake(UCameraShakeBase* Instance, bool bImmediate)
{
	if (UCameraComponent* Cam = GetViewTargetCamera())
	{
		Cam->StopCameraShake(Instance, bImmediate);
	}
}

void APlayerCameraManager::StopAllCameraShakes(bool bImmediate)
{
	if (UCameraComponent* Cam = GetViewTargetCamera())
	{
		Cam->StopAllCameraShakes(bImmediate);
	}
}

void APlayerCameraManager::PlayHitEffect(float Intensity, float Duration)
{
	if (UCameraComponent* Cam = GetViewTargetCamera())
	{
		Cam->AddHitEffect(Intensity, Duration);
	}
}
