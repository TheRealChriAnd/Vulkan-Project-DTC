#include "ShaderVK.h"
#include "DeviceVK.h"
#include <fstream>

ShaderVK::ShaderVK(DeviceVK* device, const std::string& filename, VkShaderStageFlagBits type)
{
	size_t index = filename.find_last_of('.');

	m_pDevice = device;
	m_FileName = filename.substr(0, index);
	m_SrcData = readSrcFile(filename);
	m_ShaderInfo = {};
	m_ShaderInfo.stage = type;
}

ShaderVK::~ShaderVK()
{
	vkDestroyShaderModule(m_pDevice->getDevice(), m_ShaderInfo.module, nullptr);
}

void ShaderVK::insert(const std::string& text)
{
	m_SrcData = text + m_SrcData;
}

void ShaderVK::append(const std::string& text)
{
	m_SrcData += text;
}

void ShaderVK::compile()
{
	std::string typeString;
	if (m_ShaderInfo.stage == VK_SHADER_STAGE_VERTEX_BIT)
		typeString = ".vert";
	else if (m_ShaderInfo.stage == VK_SHADER_STAGE_FRAGMENT_BIT)
		typeString = ".frag";

	std::ofstream file;
	file.open(m_FileName + "_" + typeString, std::ofstream::trunc);
	file << m_SrcData;
	file.close();

	std::string command = "shaders\\glslc.exe " + m_FileName + "_" + typeString + " -o " + m_FileName + ".spv";
	system(command.c_str());

	auto shaderCode = readFile(m_FileName + ".spv");

	VkShaderModule shaderModule = createShaderModule(m_pDevice, shaderCode);

	m_ShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_ShaderInfo.module = shaderModule;
	m_ShaderInfo.pName = "main";
}

std::string ShaderVK::readSrcFile(const std::string& filename)
{
	std::ifstream file(filename);
	std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
	file.close();
	return content;
}

std::vector<char> ShaderVK::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("Error: Failed to open file!");

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}

VkShaderModule ShaderVK::createShaderModule(DeviceVK* device, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device->getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create shader module!");

	return shaderModule;
}

VkPipelineShaderStageCreateInfo ShaderVK::getShaderInfo() const
{
	return m_ShaderInfo;
}
