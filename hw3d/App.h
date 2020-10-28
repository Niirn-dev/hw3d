#pragma once

#include "Window.h"
#include <optional>
#include <string>
#include "ChiliTimer.h"
#include "ImguiManager.h"

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
	std::vector<std::unique_ptr<class Drawable>> drawables;
	bool show_demo_window = true;
};

