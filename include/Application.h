#pragma once

#include "Core/Core.h"
#include <string>
#include "Core/Window.h"
#include <memory>

class Application
{
public:
	Application();
	~Application();

	int Init(const std::string& title, int width, int height, bool fullscreen = false, bool vsync = true);
	void Run();

	virtual void Update(float delta);
	virtual void FixedUpdate(float delta);
	virtual void Render();
	virtual void RenderImGui();

private:
	std::unique_ptr<Window> m_window;

	bool m_running;
};
