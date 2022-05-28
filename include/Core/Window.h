#pragma once

#include <string>
#include <functional>

class Window
{
public:
	Window(const std::string& title, int width, int height, bool fullscreen, bool vsync);
	~Window();

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	const std::string& GetTitle() const { return m_title; }
	void* GetNativeWindowPtr() const { return m_nativeWindowPtr; }

	void SetSize(int width, int height);
	void SetTitle(const std::string& title);
	void SetFullscreen(bool fullscreen);
	void SetVsync(bool vsync);

	void SwapBuffers();

	// callbacks

	void SetCloseCallback(const std::function<void(Window&)>& callback);
	void SetResizeCallback(const std::function<void(Window&, int, int)>& callback);
	void SetFiledropCallback(const std::function<void(Window&, int, const char**)>& callback);

private:
	std::string m_title;
	void* m_nativeWindowPtr;
	int m_width, m_height;
	bool m_fullscreen, m_vsync;

	// callbacks

	std::function<void(Window&)> m_closeCallback;
	std::function<void(Window&, int, int)> m_resizeCallback;
	std::function<void(Window&, int, const char**)> m_filedropCallback;
};