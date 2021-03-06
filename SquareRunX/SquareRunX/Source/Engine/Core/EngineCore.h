#pragma once
#include "Engine/Debug/PerformanceCounter.h"

class EngineCore
{
private:
	mutable PerformanceCounter PerfCounter;
private:
	void InitSingletons();

	void MainLoop();
	void UpdateTick(const double& DeltaTime);
	void RenderTick() const;
public:
	EngineCore();
	~EngineCore();
};