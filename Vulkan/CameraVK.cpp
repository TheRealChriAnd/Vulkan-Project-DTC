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
	m_CurrentNode = 0;
	m_Timer = 0;
	m_TotalTime = 0;
	m_FollowPath = false;

	InputVK::addKeyListener(this);
	InputVK::addMouseListener(this);
}

CameraVK::CameraVK()
{
	m_Position = glm::vec3(0.0f, 2.0f, 5.0f);
	m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_ViewMatrix = glm::lookAt(m_Position, m_Front, m_Up);
	m_Yaw = -glm::pi<float>() / 2.0f;
	m_Pitch = 0;

	m_Sensitivity = 0.005f;
	m_CameraSpeed = 0.75f;
	m_CurrentNode = 0;
	m_Timer = 0;
	m_TotalTime = 0;
	m_FollowPath = false;

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

void CameraVK::addNode(const glm::vec3& pos, const glm::vec3& target)
{
	m_PosTargetTable.push_back({ pos, target });
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
	if (InputVK::isCursorEnabled())
		return;

	m_Yaw += offset.x * m_Sensitivity;
	m_Pitch -= offset.y * m_Sensitivity;

	m_Pitch = glm::clamp(m_Pitch, -glm::pi<float>() / 2.0f + 0.0001f, glm::pi<float>() / 2.0f - 0.0001f);


	m_Direction.x = glm::cos(m_Yaw) * glm::cos(m_Pitch);
	m_Direction.y = glm::sin(m_Pitch);
	m_Direction.z = glm::sin(m_Yaw) * glm::cos(m_Pitch);

	m_Front = glm::normalize(m_Direction);
}

void CameraVK::onKeyPressed(int key)
{

}

void CameraVK::onKeyReleased(int key)
{
}

void CameraVK::moveCamera(std::pair<glm::vec3, glm::vec3> node0, std::pair<glm::vec3, glm::vec3> node1, float percentage)
{
	m_Position = lerp(node0.first, node1.first, percentage);
	m_Front = lerp(node0.second, node1.second, percentage) - m_Position;
}

glm::vec3 CameraVK::lerp(const glm::vec3& a, const glm::vec3& b, float percentage)
{
	return  a + (b - a) * percentage;
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

	if (m_FollowPath)
	{
		followPath(delta);
	}

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void CameraVK::startFollowPath()
{
	m_FollowPath = true;
	calcTotalTime();
}

void CameraVK::stopFollowPath()
{
	m_FollowPath = false;
}

void CameraVK::followPath(float delta)
{
	m_Timer += delta;

	moveCamera(getCurrentNode(), getNextNode(), m_Timer / m_TotalTime);
	if (m_Timer >= m_TotalTime)
	{
		m_Timer -= m_TotalTime;
		m_CurrentNode++;
		if (m_CurrentNode == m_PosTargetTable.size())
		{
			m_CurrentNode = 0;
			//m_FollowPath = false;
		}
		calcTotalTime();
	}
}

void CameraVK::calcTotalTime()
{
	std::pair<glm::vec3, glm::vec3> node0 = getCurrentNode();
	std::pair<glm::vec3, glm::vec3> node1 = getNextNode();
	glm::vec3 diff = node1.first - node0.first;
	float length = glm::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
	m_TotalTime = length / m_CameraSpeed;
}

const std::pair<glm::vec3, glm::vec3>& CameraVK::getCurrentNode() const
{
	return m_PosTargetTable[m_CurrentNode];
}

const std::pair<glm::vec3, glm::vec3>& CameraVK::getNextNode() const
{
	if (m_CurrentNode == m_PosTargetTable.size() - 1)
	{
		return m_PosTargetTable[0];
	}

	return m_PosTargetTable[m_CurrentNode + 1];
}
