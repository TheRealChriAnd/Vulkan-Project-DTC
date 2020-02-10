#include "Input.h"

Input::Input()
{
}

Input::~Input()
{
}

void Input::init(GLFWwindow* window) {
	glfwSetKeyCallback(window, keyInput);
	//glfwSetCursorPosCallback(window, mouseInput);
}

void Input::keyInput(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods)
{

	if (action == GLFW_PRESS) {
		m_Keys[key] = true;	
		for (IKeyListener* listener : m_KeyListeners)
			listener->onKeyPressed(key);
	}
	else if (action == GLFW_RELEASE) {
		m_Keys[key] = false;
		for (IKeyListener* listener : m_KeyListeners)
			listener->onKeyReleased(key);
	}
}

void Input::addKeyListener(IKeyListener* listener)
{
	m_KeyListeners.insert(listener);
}

void Input::removeKeyListener(IKeyListener* listener) 
{
	m_KeyListeners.erase(listener);
}

bool Input::isKeyDown(unsigned int key)
{
	return m_Keys[key];
}

glm::vec3 Input::getMousePosition()
{
	return glm::vec3();
}
