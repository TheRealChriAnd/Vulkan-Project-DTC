#include "WindowVK.h"
#include "DeviceVK.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

WindowVK::WindowVK(const char* title, unsigned int width, unsigned int height)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window, this);
}

WindowVK::~WindowVK()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

int WindowVK::getFramebufferWidth() const
{
	int width = 0;
	glfwGetFramebufferSize(m_Window, &width, nullptr);
	return width;
}

int WindowVK::getFramebufferHeight() const
{
	int height = 0;
	glfwGetFramebufferSize(m_Window, nullptr, &height);
	return height;
}

void WindowVK::setTitle(const char* title) const
{
	glfwSetWindowTitle(m_Window, title);
}

bool WindowVK::shouldClose() const
{
	return glfwWindowShouldClose(m_Window);
}

void WindowVK::pollEvents() const
{
	glfwPollEvents();
}

void WindowVK::createSurface(DeviceVK* device) 
{
	if (glfwCreateWindowSurface(device->getInstance(), m_Window, nullptr, &device->m_Surface) != VK_SUCCESS) 
		throw std::runtime_error("Error: Failed to create window surface!");
}

HWND WindowVK::GetWin32Handle() const
{
	/*SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(m_Window, &info);
	return info.info.win.window;*/
	return 0;
}