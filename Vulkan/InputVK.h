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
	static void mouseInput(GLFWwindow*, double, double);
	//void onMouseMove();

	static void addKeyListener(IKeyListener* listener);
	static void removeKeyListener(IKeyListener* listener);
	static void addMouseListener(IMouseListener* listener);
	static void removeMouseListener(IMouseListener* listener);
	static bool isKeyDown(unsigned int key);
	static const glm::vec2& getMousePosition();

private:
	InputVK();
	static void init(WindowVK* window);
private:
	static bool m_Keys[];
	static std::set<IKeyListener*> m_KeyListeners;
	static std::set<IMouseListener*> m_MouseListeners;

	static WindowVK* m_Window;

	static glm::vec2 m_LastPos;
};