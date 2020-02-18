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
	LightPoint(const glm::vec3& pos, const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec, const glm::vec3& clq);
	virtual ~LightPoint();

	Light getPointLight() const;
	void setAmbientColor(const glm::vec4& color);
	void setSpecColor(const glm::vec4& color);
	void setDiffuseColor(const glm::vec4& color);

private:
	Light m_PointLight;
};
