#pragma once

#include "Window.h"
#include <optional>
#include <string>
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"

class App
{
public:
	App( std::optional<int> wndWidth = std::nullopt,
		 std::optional<int> wndHeight = std::nullopt,
		 std::optional<std::string> wndName = std::nullopt );
	App( const App& ) = delete;
	App& operator=( const App& ) = delete;
	~App();

	int Go();
private:
	void DoFrame();
private:
	static constexpr int wndWidthDefault = 800;
	static constexpr int wndHeightDefault = 600;

	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	Camera cam;
	std::vector<std::unique_ptr<class Drawable>> drawables;

	struct
	{
		float r = 0.07f;
		float g = 0.0f;
		float b = 0.12f;
	} bkgColor;
	float simulationSpeedFactor = 1.0f;
};

