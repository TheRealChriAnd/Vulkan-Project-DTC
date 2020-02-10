#pragma once
#include "Defines.h"
#include "glm/glm.hpp"

class CameraVK
{
public:

public:

	CameraVK(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 2.0f));
	~CameraVK();

	void addPosition(glm::vec3 position);
	glm::mat4 getView() const;

private:
	glm::vec3 m_Position;
	glm::vec3 m_target;
	glm::vec3 m_Direction;
	glm::vec3 m_Front;
	glm::vec3 m_Up;

	glm::mat4 m_ViewMatrix;

	float m_CameraSpeed;
	float m_Yaw;
	float m_Pitch;

	bool m_InitMousePos;


private:

	void createCamera(glm::vec3 position, glm::vec3 target, glm::vec3 direction); // maybe not yes?
	void updateCamera(float mouseXPos, float mouseYPos);

};