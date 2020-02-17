#define STB_IMAGE_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include "stb_image.h"
#include <crtdbg.h>
#include "VulkanDemo.h"
#include "HelloRT.h"

int main()
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Application* application = new HelloRT();
	application->run();
	delete application;
	return 0;
};