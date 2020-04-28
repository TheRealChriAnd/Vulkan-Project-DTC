#define STB_IMAGE_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include "stb_image.h"
#include <crtdbg.h>
#include "VulkanDemo.h"

#include <chrono>
#include <thread>

void run(int tvs)
{
	using namespace std::chrono_literals;

	Application* application = new VulkanDemo(tvs);
	application->run();
	delete application;
}

int main()
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	/*run(1);
	run(5);
	run(10);
	run(20);
	run(30);*/
	run(40);

	return 0;
};