#include "FrameContext.h"
#include "Component/CameraComponent.h"
#include "Viewport/Viewport.h"

void FFrameContext::SetCameraInfo(const UCameraComponent* Camera)
{
	View            = Camera->GetViewMatrix();
	Proj            = Camera->GetProjectionMatrix();
	CameraPosition  = Camera->GetWorldLocation();
	CameraForward   = Camera->GetForwardVector();
	CameraRight     = Camera->GetRightVector();
	CameraUp        = Camera->GetUpVector();
	bIsOrtho        = Camera->IsOrthogonal();
	OrthoWidth      = Camera->GetOrthoWidth();
	NearClip        = Camera->GetCameraState().NearZ;
	FarClip         = Camera->GetCameraState().FarZ;

	// Per-viewport frustum — used by RenderCollector for inline frustum culling
	FrustumVolume.UpdateFromMatrix(View * Proj);
}

void FFrameContext::SetCameraInfo(const FMinimalViewInfo& POV)
{
	const FRotator Rotation = POV.Rotation;

	View = FMatrix::MakeViewMatrix(
		Rotation.GetRightVector(),
		Rotation.GetUpVector(),
		Rotation.GetForwardVector(),
		POV.Location);

	// Override aspect ratio if letterboxing is active
	const float ProjectionAspect = POV.bConstrainAspectRatio
		? POV.LetterBoxingAspectW / POV.LetterBoxingAspectH
		: POV.AspectRatio;

	if (!POV.bIsOrthogonal)
	{
		Proj = FMatrix::PerspectiveFovLH(POV.FOV, ProjectionAspect, POV.NearZ, POV.FarZ);
	}
	else
	{
		// Override aspect ratio if letterboxing is active
		const float HalfW = POV.OrthoWidth * 0.5f;
		const float HalfH = HalfW / ProjectionAspect;
		Proj = FMatrix::OrthoLH(HalfW * 2.0f, HalfH * 2.0f, POV.NearZ, POV.FarZ);
	}

	CameraPosition = POV.Location;
	CameraForward = Rotation.GetForwardVector();
	CameraRight = Rotation.GetRightVector();
	CameraUp = Rotation.GetUpVector();
	bIsOrtho = POV.bIsOrthogonal;
	OrthoWidth = POV.OrthoWidth;
	NearClip = POV.NearZ;
	FarClip = POV.FarZ;

	FrustumVolume.UpdateFromMatrix(View * Proj);
}

void FFrameContext::ApplyConstrainedAR(float TargetAspect) {
	ViewRectX		= 0.0f;
	ViewRectY		= 0.0f;
	ViewRectWidth	= ViewportWidth;
	ViewRectHeight	= ViewportHeight;

	const float CurrentAspect = ViewportWidth / ViewportHeight;

	if (CurrentAspect > TargetAspect)
	{
		ViewRectWidth = ViewportHeight * TargetAspect;
		ViewRectX = (ViewportWidth - ViewRectWidth) * 0.5f;
	}
	else
	{
		ViewRectHeight = ViewportWidth / TargetAspect;
		ViewRectY = (ViewportHeight - ViewRectHeight) * 0.5f;
	}
}

void FFrameContext::SetViewportInfo(const FViewport* VP)
{
	ViewportWidth    = static_cast<float>(VP->GetWidth());
	ViewportHeight   = static_cast<float>(VP->GetHeight());
	ViewportRTV             = VP->GetRTV();
	ViewportDSV             = VP->GetDSV();
	SceneColorCopySRV       = VP->GetSceneColorCopySRV();
	SceneColorCopyTexture   = VP->GetSceneColorCopyTexture();
	ViewportRenderTexture   = VP->GetRTTexture();
	DepthTexture            = VP->GetDepthTexture();
	DepthCopyTexture        = VP->GetDepthCopyTexture();
	DepthCopySRV            = VP->GetDepthCopySRV();
	StencilCopySRV          = VP->GetStencilCopySRV();
	NormalRTV               = VP->GetNormalRTV();
	NormalSRV               = VP->GetNormalSRV();
	CullingHeatmapRTV       = VP->GetCullingHeatmapRTV();
	CullingHeatmapSRV       = VP->GetCullingHeatmapSRV();
}
