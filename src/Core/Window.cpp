#include "Core/Window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// WINDOW CLASS

Window::Window(const std::string& title, int width, int height, bool fullscreen, bool vsync)
{
	// init properties

	m_title = title;
	m_width = width;
	m_height = height;
	m_fullscreen = fullscreen;
	m_vsync = vsync;

	// init glfw

	if (!glfwInit())
	{
		std::cout << "GLFW FAILED" << std::endl;
		return;
	}

	// window hints

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);// the window will stay hidden after creation
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // the window will be resizable
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // set opengl 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // core profile

	// create window

	m_nativeWindowPtr = glfwCreateWindow(width, height, title.c_str(), fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

	// set window user pointer to be this window

	glfwSetWindowUserPointer((GLFWwindow*)m_nativeWindowPtr, this);

	// Make the OpenGL context current

	glfwMakeContextCurrent((GLFWwindow*)m_nativeWindowPtr);

	// set vsync

	glfwSwapInterval(vsync);

	// make window visible

	glfwShowWindow((GLFWwindow*)m_nativeWindowPtr);

	// init glew

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW FAILED" << std::endl;
		return;
	}
}

Window::~Window()
{
	glfwDestroyWindow((GLFWwindow*)m_nativeWindowPtr);
	glfwTerminate();
}

void Window::SetSize(int width, int height)
{
	m_width = width;
	m_height = height;

	glfwSetWindowSize((GLFWwindow*)m_nativeWindowPtr, width, height);
}

void Window::SetTitle(const std::string& title)
{
	m_title = title;

	glfwSetWindowTitle((GLFWwindow*)m_nativeWindowPtr, title.c_str());
}

void Window::SetFullscreen(bool fullscreen)
{
	m_fullscreen = fullscreen;

	if (fullscreen)
		glfwSetWindowMonitor((GLFWwindow*)m_nativeWindowPtr, glfwGetPrimaryMonitor(), 0, 0, m_width, m_height, 0);
	else
		glfwSetWindowMonitor((GLFWwindow*)m_nativeWindowPtr, nullptr, 0, 0, m_width, m_height, 0);
}

void Window::SetVsync(bool vsync)
{
	m_vsync = vsync;

	glfwSwapInterval(vsync);
}

void Window::SwapBuffers()
{
	glfwSwapBuffers((GLFWwindow*)m_nativeWindowPtr);
}

void Window::SetCloseCallback(const std::function<void(Window&)>& callback)
{
	m_closeCallback = callback;

	glfwSetWindowCloseCallback((GLFWwindow*)m_nativeWindowPtr, [](GLFWwindow* window) {
		Window& this_window = *(Window*)glfwGetWindowUserPointer(window);

		auto& callback = this_window.m_closeCallback;

		if (callback)
			callback(this_window);
	});
}

void Window::SetResizeCallback(const std::function<void(Window&, int, int)>& callback)
{
	m_resizeCallback = callback;

	glfwSetFramebufferSizeCallback((GLFWwindow*)m_nativeWindowPtr, [](GLFWwindow* window, int width, int height) {
		Window& this_window = *(Window*)glfwGetWindowUserPointer(window);

		auto& callback = this_window.m_resizeCallback;

		if (callback)
			callback(this_window, width, height);
	});
}

void Window::SetFiledropCallback(const std::function<void(Window&, int, const char**)>& callback)
{
	m_filedropCallback = callback;

	glfwSetDropCallback((GLFWwindow*)m_nativeWindowPtr, [](GLFWwindow* window, int count, const char** paths) {
		Window& this_window = *(Window*)glfwGetWindowUserPointer(window);

		auto& callback = this_window.m_filedropCallback;

		if (callback)
			callback(this_window, count, paths);
	});
}
