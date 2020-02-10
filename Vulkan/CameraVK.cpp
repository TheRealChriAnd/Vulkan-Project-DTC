#include "CameraVK.h"
#include <glm/gtc/matrix_transform.hpp>

CameraVK::CameraVK(glm::vec3 pos) : m_InitMousePos(true)
{
	m_Position	= pos;
	m_Front		= glm::vec3(0.0f, 0.0f, -1.0f);
	m_Up		= glm::vec3(0.0f, 1.0f, 0.0f);

	m_CameraSpeed = 0.05f;
}

CameraVK::~CameraVK()
{
}

void CameraVK::addPosition(glm::vec3 position)
{
	m_Position += position;
}

glm::mat4 CameraVK::getView() const
{
	return m_ViewMatrix;
}

void CameraVK::createCamera(glm::vec3 position, glm::vec3 target, glm::vec3 direction)//ta bort
{

}

void CameraVK::updateCamera(float mouseXPos, float mouseYPos)
{
	float lastX;
	float lastY;

	if (m_InitMousePos)
	{
		lastX = mouseXPos;
		lastY = mouseYPos;
		m_InitMousePos = false;
	}

	float xoffset = mouseXPos - lastX;
	float yoffset = lastY - mouseYPos;
	lastX = mouseXPos;
	lastY = mouseYPos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	m_Yaw += xoffset;
	m_Pitch += yoffset;

	if (m_Pitch > 89.0f)
		m_Pitch = 89.0f;
	if (m_Pitch < -89.0f)
		m_Pitch = -89.0f;

	m_Direction.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
	m_Direction.y = sin(glm::radians(m_Pitch));
	m_Direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

	m_Front = glm::normalize(m_Direction);

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}
