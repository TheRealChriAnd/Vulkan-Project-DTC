#include "InputVK.h"
#include "WindowVK.h"
#include "IKeyListener.h"
#include "IMouseListener.h"
#include "GLFW/glfw3.h"

bool InputVK::m_Keys[1024];
std::set<IKeyListener*> InputVK::m_KeyListeners;

InputVK::InputVK()
{
}

InputVK ::~InputVK()
{
}

void InputVK::init(WindowVK* window) {
	glfwSetKeyCallback(window->getHandle(), keyInput);
	//glfwSetCursorPosCallback(window, mouseInput);
}

void InputVK::keyInput(
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

void InputVK::addKeyListener(IKeyListener* listener)
{
	m_KeyListeners.insert(listener);
}

void InputVK::removeKeyListener(IKeyListener* listener)
{
	m_KeyListeners.erase(listener);
}

bool InputVK::isKeyDown(unsigned int key)
{
	return m_Keys[key];
}

glm::vec3 InputVK::getMousePosition()
{
	return glm::vec3();
}
