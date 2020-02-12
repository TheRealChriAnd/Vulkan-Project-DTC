#include "CameraVK.h"
#include <glm/gtc/matrix_transform.hpp>
#include "InputVK.h"
#include <iostream>

CameraVK::CameraVK(glm::vec3 pos, float sensitivity, float speed)
{
	m_Position	= pos;
	m_Front		= glm::vec3(0.0f, 0.0f, -1.0f);
	m_Up		= glm::vec3(0.0f, 1.0f, 0.0f);
	m_ViewMatrix = glm::lookAt(m_Position, m_Front, m_Up);
	m_Yaw = glm::pi<float>()/2.0f;
	m_Pitch = glm::pi<float>();

	m_Sensitivity = sensitivity;
	m_CameraSpeed = speed;


	InputVK::addKeyListener(this);
	InputVK::addMouseListener(this);
}

CameraVK::CameraVK()
{
	m_Position = glm::vec3(0.0f, 0.0f, 2.0f);
	m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_ViewMatrix = glm::lookAt(m_Position, m_Front, m_Up);
	m_Yaw = -glm::pi<float>() / 2.0f;
	m_Pitch = 0;


	m_Sensitivity = 0.005f;
	m_CameraSpeed = 0.005f;

	onMouseMove(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f));


	InputVK::addKeyListener(this);
	InputVK::addMouseListener(this);
}

CameraVK::~CameraVK()
{
	InputVK::removeKeyListener(this);
	InputVK::removeMouseListener(this);
}

const glm::vec3& CameraVK::getPosition() const
{
	return m_Position;
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

void CameraVK::onMouseMove(const glm::vec2& pos, const glm::vec2& offset)
{
	m_Yaw += offset.x * m_Sensitivity;
	m_Pitch -= offset.y * m_Sensitivity;

	m_Pitch = glm::clamp(m_Pitch, -glm::pi<float>() / 2.0f + 0.0001f, glm::pi<float>() / 2.0f - 0.0001f);


	m_Direction.x = glm::cos(m_Yaw) * glm::cos(m_Pitch);
	m_Direction.y = glm::sin(m_Pitch);
	m_Direction.z = glm::sin(m_Yaw) * glm::cos(m_Pitch);

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
	if (InputVK::isKeyDown(KEY_UP)|| InputVK::isKeyDown(KEY_W))
	{
		m_Position += delta * m_Front;
	}
	else if (InputVK::isKeyDown(KEY_DOWN) || InputVK::isKeyDown(KEY_S))
	{
		m_Position -= delta * m_Front;
	}
	else if (InputVK::isKeyDown(KEY_RIGHT) || InputVK::isKeyDown(KEY_D))
	{
		m_Position += glm::normalize(glm::cross(m_Front, m_Up)) * delta;
	}
	else if (InputVK::isKeyDown(KEY_LEFT) || InputVK::isKeyDown(KEY_A))
	{
		m_Position -= glm::normalize(glm::cross(m_Front, m_Up)) * delta;
	}

	updateCamera();
}

void CameraVK::updateCamera()
{
	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}
