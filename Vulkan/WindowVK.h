#pragma once
#include "Defines.h"
#include <string>

class GLFWwindow;
class DeviceVK;

class WindowVK
{
	friend class DeviceVK;
public:
	WindowVK(const char* title, unsigned int width, unsigned int height);
	~WindowVK();

	int getFramebufferWidth() const;
	int getFramebufferHeight() const;
	void setTitle(const char* title) const;
	bool shouldClose() const;
	void pollEvents() const;
	HWND GetWin32Handle() const;

private:
	void createSurface(DeviceVK* device);

private:
	GLFWwindow* m_Window;

};