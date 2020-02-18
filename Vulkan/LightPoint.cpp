#include "LightPoint.h"
#include <typeinfo>


LightPoint::LightPoint(const glm::vec3& pos, const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec, const glm::vec3& clq) : LightVK(typeid(this).hash_code())
{
	m_PointLight.lightPos = glm::vec4(pos, 1.0f);
	m_PointLight.ambient = glm::vec4(amb, 1.0f);
	m_PointLight.diffuse = glm::vec4(diff, 1.0f);
	m_PointLight.specular = glm::vec4(spec, 1.0f);
	m_PointLight.CLQ = glm::vec4(clq, 1.0f);
}

LightPoint::~LightPoint()
{
}

Light LightPoint::getPointLight() const
{
	return m_PointLight;
}

void LightPoint::setAmbientColor(const glm::vec4 & color)
{
	m_PointLight.ambient = color;
}

void LightPoint::setSpecColor(const glm::vec4 & color)
{
	m_PointLight.specular = color;
}

void LightPoint::setDiffuseColor(const glm::vec4 & color)
{
	m_PointLight.diffuse = color;
}