#include "InputVK.h"
#include "WindowVK.h"
#include "IKeyListener.h"
#include "IMouseListener.h"
#include "GLFW/glfw3.h"

bool InputVK::m_Keys[1024];
std::set<IKeyListener*> InputVK::m_KeyListeners;
std::set<IMouseListener*> InputVK::m_MouseListeners;
WindowVK* InputVK::m_Window = nullptr;
glm::vec2 InputVK::m_LastPos;

InputVK::InputVK()
{

}

InputVK ::~InputVK()
{
}

void InputVK::init(WindowVK* window) 
{
	m_Window = window;
	glfwSetKeyCallback(m_Window->getHandle(), keyInput);
	glfwSetCursorPosCallback(m_Window->getHandle(), mouseInput);
	glfwSetInputMode(m_Window->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	m_LastPos = getMousePosition();
}

void InputVK::keyInput(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods)
{

	if (action == GLFW_PRESS) 
	{
		m_Keys[key] = true;	
		for (IKeyListener* listener : m_KeyListeners)
			listener->onKeyPressed(key);
	}
	else if (action == GLFW_RELEASE) 
	{
		m_Keys[key] = false;
		for (IKeyListener* listener : m_KeyListeners)
			listener->onKeyReleased(key);
	}
}


bool first = true;


void InputVK::mouseInput(GLFWwindow* window, double xpos, double ypos) 
{
	glm::vec2 pos(xpos, ypos);
	glm::vec2 offset = pos - m_LastPos;
	if (first)
	{
		offset = glm::vec2(0.0f, 0.0f);
		first = false;
	}
	for (IMouseListener* listener : m_MouseListeners)
		listener->onMouseMove(pos, offset);
	m_LastPos = pos;
}

void InputVK::addKeyListener(IKeyListener* listener)
{
	m_KeyListeners.insert(listener);
}

void InputVK::removeKeyListener(IKeyListener* listener)
{
	m_KeyListeners.erase(listener);
}

void InputVK::addMouseListener(IMouseListener* listener) 
{
	m_MouseListeners.insert(listener);
}

void InputVK::removeMouseListener(IMouseListener* listener) 
{
	m_MouseListeners.erase(listener);
}

bool InputVK::isKeyDown(unsigned int key)
{
	return m_Keys[key];
}

const glm::vec2& InputVK::getMousePosition()
{
	double x;
	double y;
	int wx;
	int wy;
	glfwGetWindowPos(m_Window->getHandle(), &wx, &wy);
	glfwGetCursorPos(m_Window->getHandle(), &x, &y);
	return glm::vec2(x + wx, y + wy);
}
