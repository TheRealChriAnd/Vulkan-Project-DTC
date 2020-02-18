#pragma once
#include "LightVK.h"


struct Light
{
	glm::vec4 lightPos;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 CLQ;
};


class LightPoint : public LightVK
{
public:
	LightPoint(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 clq);
	virtual ~LightPoint();

	Light getPointLight() const;
	void setAmbientColor(glm::vec3 color);

private:
	Light m_PointLight;
};
