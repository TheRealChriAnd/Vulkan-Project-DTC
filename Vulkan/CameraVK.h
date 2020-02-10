#pragma once
#include "Defines.h"
#include "IMouseListener.h"
#include "IKeyListener.h"
#include "glm/glm.hpp"

#define KEY_W 87
#define KEY_S 83
#define KEY_A 65
#define KEY_D 68

#define KEY_UP 265
#define KEY_DOWN 264
#define KEY_RIGHT 262
#define KEY_LEFT 263

class Input;

class CameraVK : public IMouseListener, IKeyListener
{
public:

public:

	CameraVK(Input* input, glm::vec3 pos, float sensitivity, float speed);
	CameraVK(Input* input);
	~CameraVK();

	glm::mat4 getView() const;

	virtual void onMouseButtonPressed(int button) override;
	virtual void onMouseButtonRelease(int button) override;
	virtual void onMouseMove(glm::vec2 pos, glm::vec2 offset) override;

	virtual void onKeyPressed(int key) override;
	virtual void onKeyReleased(int key) override;

private:

	Input* m_Input;

	glm::vec3 m_Position;
	glm::vec3 m_target;
	glm::vec3 m_Direction;
	glm::vec3 m_Front;
	glm::vec3 m_Up;

	glm::mat4 m_ViewMatrix;

	float m_Sensitivity;
	float m_CameraSpeed;
	float m_Yaw;
	float m_Pitch;


private:

	void createCamera(glm::vec3 position, glm::vec3 target, glm::vec3 direction); // maybe not yes?'
	void update(float delta);
	void updateCamera();

};