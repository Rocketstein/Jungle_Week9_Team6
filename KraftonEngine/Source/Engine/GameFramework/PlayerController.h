#pragma once

#include "GameFramework/AActor.h"
#include "Input/EnhancedInputManager.h"

class APawnActor;
class APlayerCameraManager;
struct FInputMappingContext;
struct FInputAction;

// 플레이어 입력을 받아 Pawn을 조종하는 액터.
// APlayerController 대응 자체 FEnhancedInputManager로 매핑 컨텍스트/액션 바인딩을 관리한다.
// BeginPlay 시 APlayerCameraManager를 스폰해 카메라 view를 관리한다
class APlayerController : public AActor
{
public:
	DECLARE_CLASS(APlayerController, AActor)

	APlayerController();

	void BeginPlay() override;
	void EndPlay() override;
	void Tick(float DeltaTime) override;

	void Possess(APawnActor* InPawn);
	void UnPossess();
	APawnActor* GetPawn() const { return PossessedPawn; }

	APlayerCameraManager* GetPlayerCameraManager() const { return PlayerCameraManager; }

	// 서브클래스가 BeginPlay 시점에 매핑 컨텍스트/액션 바인딩을 등록하기 위한 훅 (UE의 SetupPlayerInputComponent 대응).
	virtual void SetupInputComponent() {}

	// 매핑/바인딩 헬퍼 — 서브클래스가 SetupInputComponent에서 호출.
	void AddMappingContext(FInputMappingContext* Context, int32 Priority = 0)
	{
		EnhancedInput.AddMappingContext(Context, Priority);
	}
	void RemoveMappingContext(FInputMappingContext* Context)
	{
		EnhancedInput.RemoveMappingContext(Context);
	}
	void BindAction(FInputAction* Action, ETriggerEvent TriggerEvent, FInputActionCallback Callback)
	{
		EnhancedInput.BindAction(Action, TriggerEvent, std::move(Callback));
	}

protected:
	// PCM 클래스 이름  서브클래스가 ctor에서 변경 가능 
	FString PlayerCameraManagerClassName = "APlayerCameraManager";

	APawnActor* PossessedPawn = nullptr;
	APlayerCameraManager* PlayerCameraManager = nullptr;
	FEnhancedInputManager EnhancedInput;
};
