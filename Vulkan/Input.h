#pragma once
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "IKeyListener.h"
#include "IMouseListener.h"
#include <set>


class Input 
{
public:
	~Input();
	
	static void keyInput(
		GLFWwindow* window, 
		int key, 
		int scancode, 
		int action, 
		int mods);
	//void cameraInput();
	//void onMouseMove();

	static void addKeyListener(IKeyListener* listener);
	static void removeKeyListener(IKeyListener* listener);
	static bool isKeyDown(unsigned int key);
	static glm::vec3 getMousePosition();

private:
	Input();
	static void init(GLFWwindow* window);
private:
	static bool m_Keys[1024];
	static std::set<IKeyListener*> m_KeyListeners;
};