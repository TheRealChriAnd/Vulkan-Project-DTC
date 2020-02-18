#pragma once
#include "glm/glm.hpp"
#include <vector>

class LightVK 
{
public:
	virtual ~LightVK();

	//void createPointLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 clq);
	long long getLightType() const;

public:
	long long m_LightType;

protected:
	LightVK(long long type);

private:

	//std::vector<PointLight> m_PLights;
};