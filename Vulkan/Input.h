#pragma once
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "IKeyListener.h"
#include "IMouseListener.h"
#include <set>
//glfwSetKeyCallback(window, key_callback);
//glfwSetCursorPosCallback(window, cursor_callback);

class Input 
{
public:
	Input();
	~Input();
	
	void keyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
	//void cameraInput();
	//void onMouseMove();

	void addKeyListener(IKeyListener* listener);
	bool isKeyDown(unsigned int key) const;
	glm::vec3 getMousePosition() const;

private:
	bool* m_Keys;
	std::set<IKeyListener*> m_KeyListeners;
};