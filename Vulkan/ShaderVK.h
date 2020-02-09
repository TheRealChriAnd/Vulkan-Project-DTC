#pragma once
#include "Defines.h"
#include <string>
#include <vector>

class DeviceVK;

class ShaderVK
{
public:

	ShaderVK(DeviceVK* device, const std::string& filename, VkShaderStageFlagBits type);
	~ShaderVK();

	void insert(const std::string& text);
	void append(const std::string& text);
	void compile();

	VkPipelineShaderStageCreateInfo getShaderInfo() const;

private:
	static VkShaderModule createShaderModule(DeviceVK* device, const std::vector<char>& code);
	static std::vector<char> readFile(const std::string & filename);
	static std::string readSrcFile(const std::string& filename);
	
private:
	VkPipelineShaderStageCreateInfo m_ShaderInfo;
	std::string m_SrcData;
	std::string m_FileName;
	DeviceVK* m_pDevice;
};
