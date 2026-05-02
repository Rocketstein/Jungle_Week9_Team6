#pragma once
#include "GameFramework/AActor.h"
#include "Game/Map/MapChunkTemplate.h"

class AMapChunk : public AActor {
public:
	DECLARE_CLASS(AMapChunk, AActor)
	void BeginPlay() override;
	void EndPlay()	 override;
	void InitFromTemplate(const FMapChunkTemplate& InTemplate);

private:


};