#include "Input.h"

Input::Input()
{
	bool m_Keys[1024];
}

Input::~Input()
{
}

void Input::keyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (m_Keys[key] == GLFW_PRESS)
		m_Keys[key] = true;
	else if (m_Keys[key] == GLFW_RELEASE)
		m_Keys[key] = false;

	for (IKeyListener* listener : m_KeyListeners)
		listener->onKeyPressed(key);


}

/*void Input::onMouseMove()
{
	GLfloat cameraSpeed = 1.0f;

}*/

void Input::addKeyListener(IKeyListener* listener)
{
	m_KeyListeners.insert(listener);
}

glm::vec3 Input::getMousePosition() const
{
	return glm::vec3();
}
