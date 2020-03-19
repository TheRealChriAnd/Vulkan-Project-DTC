#pragma once
#include "Defines.h"
#include "IMouseListener.h"
#include "IKeyListener.h"
#include <unordered_map>
#include <vector>
#include <utility>
#include <set>
#include "glm/glm.hpp"

#define KEY_W 87
#define KEY_S 83
#define KEY_A 65
#define KEY_D 68

#define KEY_UP 265
#define KEY_DOWN 264
#define KEY_RIGHT 262
#define KEY_LEFT 263

class ICameraListener;

class CameraVK : public IMouseListener, IKeyListener
{
public:
	CameraVK(glm::vec3 pos, float sensitivity, float speed);
	CameraVK();
	~CameraVK();

	const glm::vec3& getPosition() const;
	const glm::mat4& getView() const;

	void addNode(const glm::vec3& pos, const glm::vec3& target);

	virtual void onMouseButtonPressed(int button) override;
	virtual void onMouseButtonRelease(int button) override;
	virtual void onMouseMove(const glm::vec2& pos, const glm::vec2& offset) override;

	virtual void onKeyPressed(int key) override;
	virtual void onKeyReleased(int key) override;

	void update(float delta);
	void startFollowPath(bool begining);
	void stopFollowPath();

	void addListener(ICameraListener* listener);
	void removeListener(ICameraListener* listener);

private:
	glm::vec3 m_Position;
	glm::vec3 m_target;
	glm::vec3 m_Direction;
	glm::vec3 m_Front;
	glm::vec3 m_Up;

	std::vector<std::pair<glm::vec3, glm::vec3>> m_PosTargetTable;

	glm::mat4 m_ViewMatrix;

	float m_Sensitivity;
	float m_CameraSpeed;
	float m_Yaw;
	float m_Pitch;
	float m_Timer;
	int m_CurrentNode;
	float m_TotalTime;
	bool m_FollowPath;

	std::set<ICameraListener*> m_Listeners;

private:

	void moveCamera(std::pair<glm::vec3, glm::vec3> node0, std::pair<glm::vec3, glm::vec3> node1, float percentage);
	glm::vec3 lerp(const glm::vec3& a, const glm::vec3& b, float percentage);
	void followPath(float delta);
	void calcTotalTime();
	const std::pair<glm::vec3, glm::vec3>& getCurrentNode() const;
	const std::pair<glm::vec3, glm::vec3>& getNextNode() const;

	void onCameraPathEnded();
};