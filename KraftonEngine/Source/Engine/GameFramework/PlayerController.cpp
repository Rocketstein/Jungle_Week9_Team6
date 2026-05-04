#include "PlayerController.h"

#include "GameFramework/PawnActor.h"
#include "GameFramework/PlayerCameraManager.h"
#include "GameFramework/World.h"
#include "Input/InputManager.h"
#include "Component/CameraComponent.h"
#include "Engine/Runtime/Engine.h"
#include "Object/Object.h"
#include "Object/ObjectFactory.h"
#include "Viewport/GameViewportClient.h"

IMPLEMENT_CLASS(APlayerController, AActor)

APlayerController::APlayerController()
{
	bNeedsTick = true;
	bTickInEditor = false;
}

void APlayerController::BeginPlay()
{
	Super::BeginPlay();

	// PCM 스폰 — 클래스명 기반으로 ObjectFactory를 통해 생성 후 World에 등록.
	if (UWorld* World = GetWorld())
	{
		UObject* Obj = FObjectFactory::Get().Create(PlayerCameraManagerClassName, World->GetCurrentLevel());
		PlayerCameraManager = Obj ? Cast<APlayerCameraManager>(Obj) : nullptr;
		if (PlayerCameraManager)
		{
			World->AddActor(PlayerCameraManager);
		}
		else if (Obj)
		{
			UObjectManager::Get().DestroyObject(Obj);
		}
	}

	SetupInputComponent();
}

void APlayerController::EndPlay()
{
	if (PlayerCameraManager)
	{
		PlayerCameraManager = nullptr; // World 액터 라이프사이클이 정리해줌
	}
	Super::EndPlay();
}

void APlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	EnhancedInput.ProcessInput(&FInputManager::Get(), DeltaTime, /*bIgnoreGui=*/false);

	// PCM이 매 프레임 카메라 view를 갱신 (PlayerController가 자기 PCM의 tick driver — UE 패턴)
	if (PlayerCameraManager)
	{
		PlayerCameraManager->UpdateCamera(DeltaTime);
	}
}

void APlayerController::Possess(APawnActor* InPawn)
{
	if (PossessedPawn == InPawn) return;
	UnPossess();
	PossessedPawn = InPawn;

	if (!PossessedPawn) return;

	UCameraComponent* PawnCamera = nullptr;
	for (UActorComponent* Comp : PossessedPawn->GetComponents())
	{
		if (UCameraComponent* Cam = Cast<UCameraComponent>(Comp))
		{
			PawnCamera = Cam;
			break;
		}
	}

	if (PawnCamera)
	{
		if (UWorld* World = GetWorld())
		{
			World->SetActiveCamera(PawnCamera);
		}

		if (GEngine)
		{
			if (UGameViewportClient* GameVC = GEngine->GetGameViewportClient())
			{
				GameVC->Possess(PawnCamera);
			}
		}
	}

	// PCM의 ViewTarget을 새 Pawn으로 — PCM이 자체적으로 카메라 컴포넌트를 찾아 사용
	if (PlayerCameraManager)
	{
		PlayerCameraManager->SetViewTarget(PossessedPawn);
	}
}

void APlayerController::UnPossess()
{
	PossessedPawn = nullptr;
	if (PlayerCameraManager)
	{
		PlayerCameraManager->SetViewTarget(nullptr);
	}
}
