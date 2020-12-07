#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
#include <set>
#include "TestPlane.h"

class App
{
public:
	App();
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame();
	void DoCameraControls( float dt ) noexcept;
	void ShowImguiDemoWindow();
private:
	bool show_demo_window = false;
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	float speed_factor = 1.0f;
	Camera cam;
	PointLight light;
	Model nano{ wnd.Gfx(),"Models\\nano_textured\\nanosuit.obj" };
	TestPlane plane{ wnd.Gfx(),5.0f };
};