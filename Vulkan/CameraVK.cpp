#include "CameraVK.h"
#include <glm/gtc/matrix_transform.hpp>
#include "InputVK.h"

CameraVK::CameraVK(glm::vec3 pos, float sensitivity, float speed)
{
	m_Position	= pos;
	m_Front		= glm::vec3(0.0f, 0.0f, -1.0f);
	m_Up		= glm::vec3(0.0f, 1.0f, 0.0f);
	m_ViewMatrix = glm::lookAt(m_Position, m_Front, m_Up);

	m_Sensitivity = sensitivity;
	m_CameraSpeed = speed;

	InputVK::addKeyListener(this);
	//Input::addMouseListener(this);
}

CameraVK::CameraVK()
{
	m_Position = glm::vec3(0.0f, 0.0f, 2.0f);
	m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_ViewMatrix = glm::lookAt(m_Position, m_Front, m_Up);

	m_Sensitivity = 0.05f;
	m_CameraSpeed = 0.05f;

	InputVK::addKeyListener(this);
	//Input::addMouseListener(this);
}

CameraVK::~CameraVK()
{
	InputVK::removeKeyListener(this);
	//Input::removeMouseListener(this);
}

const glm::mat4& CameraVK::getView() const
{
	return m_ViewMatrix;
}

void CameraVK::onMouseButtonPressed(int button)
{
	//TO DO
}

void CameraVK::onMouseButtonRelease(int button)
{
	//TO DO
}

void CameraVK::onMouseMove(glm::vec2 pos, glm::vec2 offset)
{
	offset.x *= m_Sensitivity;
	offset.y *= m_Sensitivity;

	m_Yaw += offset.x;
	m_Pitch += offset.y;

	if (m_Pitch > 89.0f)
		m_Pitch = 89.0f;
	if (m_Pitch < -89.0f)
		m_Pitch = -89.0f;

	m_Direction.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
	m_Direction.y = sin(glm::radians(m_Pitch));
	m_Direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

	m_Front = glm::normalize(m_Direction);

	updateCamera();
}

void CameraVK::onKeyPressed(int key)
{

}

void CameraVK::onKeyReleased(int key)
{
}

void CameraVK::createCamera(glm::vec3 position, glm::vec3 target, glm::vec3 direction)//ta bort
{

}

void CameraVK::update(float delta)
{
	if (Input::isKeyDown(KEY_UP)|| Input::isKeyDown(KEY_W))
	{
		m_Position += m_CameraSpeed * m_Front;
	}
	else if (Input::isKeyDown(KEY_DOWN) || Input::isKeyDown(KEY_S))
	{
		m_Position -= m_CameraSpeed * m_Front;
	}
	else if (Input::isKeyDown(KEY_RIGHT) || Input::isKeyDown(KEY_D))
	{
		m_Position += glm::normalize(glm::cross(m_Front, m_Up)) * m_CameraSpeed;
	}
	else if (Input::isKeyDown(KEY_LEFT) || Input::isKeyDown(KEY_A))
	{
		m_Position -= glm::normalize(glm::cross(m_Front, m_Up)) * m_CameraSpeed;
	}

	updateCamera();
}

void CameraVK::updateCamera()
{
	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}