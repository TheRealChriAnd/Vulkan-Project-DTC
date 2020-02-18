#include "LightVK.h"

LightVK::LightVK(long long type)
{
	m_LightType = type;
}

LightVK::~LightVK()
{
}

//void LightVK::createPointLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 clq)
//{
//	
//}

long long LightVK::getLightType() const
{
	return m_LightType;
}

