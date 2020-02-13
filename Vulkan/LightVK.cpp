#include "LightVK.h"

LightVK::LightVK(
	glm::vec3 dir, 
	glm::vec3 ambient, 
	glm::vec3 diffuse, 
	glm::vec3 specular)
{
	m_Direction = dir;
	m_Ambient = ambient;
	m_Diffuse = diffuse;
	m_Specular = specular;
}

LightVK::~LightVK()
{
}

void LightVK::setDir(glm::vec3 dir)
{
	m_Direction = dir;
}

const glm::vec3& LightVK::getDir() const
{
	return m_Direction;
}

const glm::vec3& LightVK::getAmbient() const
{
	return m_Ambient;
}

const glm::vec3& LightVK::getDiffuse() const
{
	return m_Diffuse;
}

const glm::vec3& LightVK::getSpecular() const
{
	return m_Specular;
}
