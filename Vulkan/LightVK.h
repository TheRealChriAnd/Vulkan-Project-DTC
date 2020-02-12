#pragma once
#include "glm/glm.hpp"


class LightVK 
{
public:
	LightVK(glm::vec3 dir, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	~LightVK();

	void setDir(glm::vec3 dir);
	glm::vec3 getDir() const;
	glm::vec3 getAmbient() const;
	glm::vec3 getDiffuse() const;
	glm::vec3 getSpecular() const;

private:
	glm::vec3 m_Direction;
	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;
};