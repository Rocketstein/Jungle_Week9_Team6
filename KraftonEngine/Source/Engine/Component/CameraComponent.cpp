#include "Component/CameraComponent.h"
#include "Object/ObjectFactory.h"
#include "Object/UClass.h"
#include "Camera/CameraShake.h"
#include "Camera/CameraTypes.h"
#include "Camera/CameraModifier.h"
#include "Camera/CameraModifier_CameraShake.h"
#include "Camera/CameraModifier_HitEffect.h"
#include <cmath>

IMPLEMENT_CLASS(UCameraComponent, USceneComponent)
HIDE_FROM_COMPONENT_LIST(UCameraComponent)

FMatrix UCameraComponent::GetViewMatrix() const
{
	UpdateWorldMatrix();

	// 카메라 자체 modifier(셰이크/effects)와 PCM modifier(글로벌 fade 등) 두 layer를 합산해 view 산출
	FVector FinalLoc = GetWorldLocation() + SelfAdditiveLocationOffset + ExternalAdditiveLocationOffset;
	FRotator FinalRot(GetWorldRotation());
	FinalRot.Pitch += SelfAdditiveRotationOffset.Pitch + ExternalAdditiveRotationOffset.Pitch;
	FinalRot.Yaw   += SelfAdditiveRotationOffset.Yaw   + ExternalAdditiveRotationOffset.Yaw;
	FinalRot.Roll  += SelfAdditiveRotationOffset.Roll  + ExternalAdditiveRotationOffset.Roll;

	return FMatrix::MakeViewMatrix(FinalRot.GetRightVector(), FinalRot.GetUpVector(), FinalRot.GetForwardVector(), FinalLoc);
}

FMatrix UCameraComponent::GetProjectionMatrix() const
{
	if (!CameraState.bIsOrthogonal) {
		return FMatrix::PerspectiveFovLH(CameraState.FOV, CameraState.AspectRatio, CameraState.NearZ, CameraState.FarZ);
	}
	else {
		float HalfW = CameraState.OrthoWidth * 0.5f;
		float HalfH = HalfW / CameraState.AspectRatio;
		return FMatrix::OrthoLH(HalfW * 2.0f, HalfH * 2.0f, CameraState.NearZ, CameraState.FarZ);
	}
}

FMatrix UCameraComponent::GetViewProjectionMatrix() const
{
	return GetViewMatrix() * GetProjectionMatrix();
}

FConvexVolume UCameraComponent::GetConvexVolume() const
{
	FConvexVolume ConvexVolume;
	ConvexVolume.UpdateFromMatrix(GetViewMatrix() * GetProjectionMatrix());
	return ConvexVolume;
}

void UCameraComponent::LookAt(const FVector& Target)
{
	FVector Position = GetWorldLocation();
	FVector Diff = (Target - Position).Normalized();

	constexpr float Rad2Deg = 180.0f / 3.14159265358979f;

	FRotator LookRotation = GetRelativeRotation();
	LookRotation.Pitch = -asinf(Diff.Z) * Rad2Deg;

	if (fabsf(Diff.Z) < 0.999f) {
		LookRotation.Yaw = atan2f(Diff.Y, Diff.X) * Rad2Deg;
	}

	SetRelativeRotation(LookRotation);
}

void UCameraComponent::OnResize(int32 Width, int32 Height)
{
	CameraState.AspectRatio = static_cast<float>(Width) / static_cast<float>(Height);
}

void UCameraComponent::SetCameraState(const FCameraState& NewState)
{
	CameraState = NewState;
}

FRay UCameraComponent::DeprojectScreenToWorld(float MouseX, float MouseY, float ScreenWidth, float ScreenHeight) {
	FRay Ray{};
	if (ScreenWidth <= 0.0f || ScreenHeight <= 0.0f)
	{
		Ray.Origin = GetWorldLocation();
		Ray.Direction = GetForwardVector();
		return Ray;
	}

	float NdcX = (2.0f * MouseX) / ScreenWidth - 1.0f;
	float NdcY = 1.0f - (2.0f * MouseY) / ScreenHeight;

	// Reversed-Z: near plane = 1, far plane = 0
	const FVector NdcNear(NdcX, NdcY, 1.0f);
	const FVector NdcFar(NdcX, NdcY, 0.0f);

	const FMatrix InverseViewProjection = (GetViewMatrix() * GetProjectionMatrix()).GetInverse();
	const FVector WorldNear = InverseViewProjection.TransformPositionWithW(NdcNear);
	const FVector WorldFar = InverseViewProjection.TransformPositionWithW(NdcFar);

	FVector Dir = WorldFar - WorldNear;
	const float Length = std::sqrt(Dir.X * Dir.X + Dir.Y * Dir.Y + Dir.Z * Dir.Z);
	Dir = (Length > 1e-4f) ? (Dir / Length) : GetForwardVector();

	if (CameraState.bIsOrthogonal)
	{
		Ray.Origin = WorldNear;
		Ray.Direction = GetForwardVector();
	}
	else
	{
		Ray.Origin = GetWorldLocation();
		Ray.Direction = Dir;
	}

	return Ray;
}

void UCameraComponent::BeginPlay()
{
	USceneComponent::BeginPlay();
	// 카메라 modifier 갱신은 엔진 내부 로직이라 사용자 bTickEnable과 무관하게 항상 tick
	SetComponentTickEnabled(true);
}

void UCameraComponent::EndPlay()
{
	for (UCameraModifier* Mod : CameraModifiers)
	{
		if (Mod)
		{
			Mod->OnRemoved();
			UObjectManager::Get().DestroyObject(Mod);
		}
	}
	CameraModifiers.clear();
	CachedShakeModifier = nullptr;
	CachedHitEffectModifier = nullptr;

	USceneComponent::EndPlay();
}

UCameraModifier* UCameraComponent::AddCameraModifier(UClass* ModifierClass)
{
	if (!ModifierClass) return nullptr;
	if (!ModifierClass->IsA(UCameraModifier::StaticClass())) return nullptr;
	return AddCameraModifierByName(ModifierClass->GetName());
}

UCameraModifier* UCameraComponent::AddCameraModifierByName(const char* ModifierClassName)
{
	if (!ModifierClassName) return nullptr;
	UObject* NewObj = FObjectFactory::Get().Create(ModifierClassName, this);
	UCameraModifier* NewMod = NewObj ? Cast<UCameraModifier>(NewObj) : nullptr;
	if (!NewMod)
	{
		if (NewObj) UObjectManager::Get().DestroyObject(NewObj);
		return nullptr;
	}

	CameraModifiers.push_back(NewMod);
	NewMod->OnAdded();
	return NewMod;
}

void UCameraComponent::RemoveCameraModifier(UCameraModifier* Modifier)
{
	if (!Modifier) return;
	auto It = std::find(CameraModifiers.begin(), CameraModifiers.end(), Modifier);
	if (It == CameraModifiers.end()) return;

	Modifier->OnRemoved();
	if (CachedShakeModifier == Modifier) CachedShakeModifier = nullptr;
	if (CachedHitEffectModifier == Modifier) CachedHitEffectModifier = nullptr;

	CameraModifiers.erase(It);
	UObjectManager::Get().DestroyObject(Modifier);
}

UCameraModifier_CameraShake* UCameraComponent::GetOrCreateCameraShakeModifier()
{
	if (CachedShakeModifier) return CachedShakeModifier;
	CachedShakeModifier = Cast<UCameraModifier_CameraShake>(AddCameraModifier(UCameraModifier_CameraShake::StaticClass()));
	return CachedShakeModifier;
}

UCameraModifier_HitEffect* UCameraComponent::GetOrCreateHitEffectModifier()
{
	if (CachedHitEffectModifier) return CachedHitEffectModifier;
	CachedHitEffectModifier = Cast<UCameraModifier_HitEffect>(AddCameraModifier(UCameraModifier_HitEffect::StaticClass()));
	return CachedHitEffectModifier;
}

void UCameraComponent::ApplyCameraModifiers(float DeltaTime, FMinimalViewInfo& InOutView)
{
	for (UCameraModifier* Mod : CameraModifiers)
	{
		if (!Mod || Mod->bDisabled) continue;
		if (Mod->ModifyCamera(DeltaTime, InOutView)) break; // true 반환 시 체인 중단
	}
}

UCameraShakeBase* UCameraComponent::StartCameraShake(UClass* ShakeClass, float Scale)
{
	UCameraModifier_CameraShake* ShakeMod = GetOrCreateCameraShakeModifier();
	return ShakeMod ? ShakeMod->AddCameraShake(ShakeClass, Scale) : nullptr;
}

UCameraShakeBase* UCameraComponent::StartCameraShakeByName(const char* ShakeClassName, float Scale)
{
	UCameraModifier_CameraShake* ShakeMod = GetOrCreateCameraShakeModifier();
	return ShakeMod ? ShakeMod->AddCameraShakeByName(ShakeClassName, Scale) : nullptr;
}

void UCameraComponent::StopCameraShake(UCameraShakeBase* Instance, bool bImmediate)
{
	if (CachedShakeModifier) CachedShakeModifier->RemoveCameraShake(Instance, bImmediate);
}

void UCameraComponent::StopAllCameraShakes(bool bImmediate)
{
	if (CachedShakeModifier) CachedShakeModifier->RemoveAllCameraShakes(bImmediate);
}

void UCameraComponent::AddHitEffect(float Intensity, float Duration)
{
	UCameraModifier_HitEffect* HitMod = GetOrCreateHitEffectModifier();
	if (HitMod) HitMod->AddHitEffect(Intensity, Duration);
}

UCameraShakeBase* UCameraComponent::StartCameraShake(float Intensity, float Duration)
{
	// Back-compat기존 시그니처는 SinWaveCameraShake에 Intensity/Duration을 직접 set
	UCameraShakeBase* Shake = StartCameraShakeByName("USinWaveCameraShake", 1.0f);
	if (Shake)
	{
		Shake->Intensity = Intensity;
		Shake->Duration = Duration;
	}
	return Shake;
}

void UCameraComponent::SetExternalAdditiveOffsets(const FVector& InLocOffset, const FRotator& InRotOffset, float InAdditionalHitEffect)
{
	ExternalAdditiveLocationOffset = InLocOffset;
	ExternalAdditiveRotationOffset = InRotOffset;
	ExternalHitEffectIntensity = InAdditionalHitEffect;
}

void UCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction& ThisTickFunction)
{
	USceneComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 카메라 자체 modifier 체인을 평가해 Self layer offset/HitEffect 캐시.
	// PCM이 없는 경우(Playground의 TestPlayer.lua)에도 이 경로만으로 쉐이크/히트 이펙트 동작
	FMinimalViewInfo View;
	View.Location = FVector::ZeroVector;
	View.Rotation = FRotator::ZeroRotator;
	View.HitEffectIntensity = 0.0f;

	ApplyCameraModifiers(DeltaTime, View);

	SelfAdditiveLocationOffset = View.Location;
	SelfAdditiveRotationOffset = View.Rotation;
	SelfHitEffectIntensity = View.HitEffectIntensity;
}

void UCameraComponent::GetEditableProperties(TArray<FPropertyDescriptor>& OutProps)
{
	USceneComponent::GetEditableProperties(OutProps);
	OutProps.push_back({ "FOV",         EPropertyType::Float, &CameraState.FOV, 0.1f,   3.14f,    0.01f });
	OutProps.push_back({ "Near Z",      EPropertyType::Float, &CameraState.NearZ, 0.01f,  100.0f,   0.01f });
	OutProps.push_back({ "Far Z",       EPropertyType::Float, &CameraState.FarZ, 1.0f,   100000.0f, 10.0f });
	OutProps.push_back({ "Orthographic",EPropertyType::Bool,  &CameraState.bIsOrthogonal});
	OutProps.push_back({ "Ortho Width", EPropertyType::Float, &CameraState.OrthoWidth, 0.1f,   1000.0f,  0.5f });
}
