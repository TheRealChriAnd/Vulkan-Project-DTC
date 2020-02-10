#pragma once
#include "glm/glm.hpp"

class IMouseListener 
{
public:
	virtual void onMouseButtonPressed(int button) = 0;
	virtual void onMouseButtonRelease(int button) = 0;
	virtual void onMouseMove(const glm::vec2& pos, const glm::vec2& offset) = 0;
};