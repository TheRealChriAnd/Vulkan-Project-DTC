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
	
	static void keyboardCallback(
		GLFWwindow* window, 
		int key, 
		int scancode, 
		int action, 
		int modKeys);

	static void mouseMoveCallback(GLFWwindow* window, double x, double y);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int modKeys);

	static void addKeyListener(IKeyListener* listener);
	static void removeKeyListener(IKeyListener* listener);
	static void addMouseListener(IMouseListener* listener);
	static void removeMouseListener(IMouseListener* listener);
	static bool isKeyDown(unsigned int key);
	static const glm::vec2& getMousePosition();
	static void setCursorEnabled(bool enabled);
	static bool isCursorEnabled();

private:
	InputVK();
	static void init(WindowVK* window);

private:
	static bool m_Keys[];
	static std::set<IKeyListener*> m_KeyListeners;
	static std::set<IMouseListener*> m_MouseListeners;

	static WindowVK* m_Window;

	static bool m_CursorEnabled;
	static glm::vec2 m_LastPos;
};