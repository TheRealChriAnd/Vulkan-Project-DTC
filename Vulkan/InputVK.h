#pragma once
#include "glm/glm.hpp"

#include <set>

class WindowVK;
class IKeyListener;
class IMouseListener;
class GLFWwindow;

class InputVK 
{
	friend class Application;
public:
	~InputVK();
	
	static void keyInput(
		GLFWwindow* window, 
		int key, 
		int scancode, 
		int action, 
		int mods);
	//void cameraInput();
	//void onMouseMove();

	static void addKeyListener(IKeyListener* listener);
	static void removeKeyListener(IKeyListener* listener);
	static bool isKeyDown(unsigned int key);
	static glm::vec3 getMousePosition();

private:
	InputVK();
	static void init(WindowVK* window);
private:
	static bool m_Keys[];
	static std::set<IKeyListener*> m_KeyListeners;
};