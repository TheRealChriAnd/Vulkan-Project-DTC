#include "MaterialVK.h"
#include "ShaderVK.h"
#include "DeviceVK.h"
#include "SwapChainVK.h"
#include "UniformBufferVK.h"

MaterialVK::MaterialVK(DeviceVK* device, SwapChainVK* swapChain, const std::string& name)
{
	m_Device = device;
	m_SwapChain = swapChain;
	m_Name = name;
}

MaterialVK::~MaterialVK()
{
	vkDeviceWaitIdle(m_Device->getDevice());
	m_UniformBuffer->release();
	for (ShaderVK* shader : m_Shaders)
	{
		shader->release();
	}
}

const std::vector<ShaderVK*>& MaterialVK::getShaders() const
{
	return m_Shaders;
}

UniformBufferVK* MaterialVK::getUniformBuffer() const
{
	return m_UniformBuffer;
}

void MaterialVK::setShader(const std::string& shaderFileName)
{
	VkShaderStageFlagBits shaderType;
	/*switch (type)
	{
	case Material::ShaderType::VS:
		shaderType = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case Material::ShaderType::PS:
		shaderType = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	case Material::ShaderType::GS:
		shaderType = VK_SHADER_STAGE_GEOMETRY_BIT;
		break;
	case Material::ShaderType::CS:
		shaderType = VK_SHADER_STAGE_COMPUTE_BIT;
		break;
	default:
		break;
	}*/

	//m_Shaders.push_back(new ShaderVK(m_Device, shaderFileName, shaderType));
}

int MaterialVK::compileMaterial(std::string& errString)
{
	for (ShaderVK* shader : m_Shaders)
	{
		/*ShaderType shaderType;
		switch (shader->getShaderInfo().stage)
		{
		case VK_SHADER_STAGE_VERTEX_BIT:
			shaderType = Material::ShaderType::VS;
			break;
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			shaderType = Material::ShaderType::PS;
			break;
		case VK_SHADER_STAGE_GEOMETRY_BIT:
			shaderType = Material::ShaderType::GS;
			break;
		case VK_SHADER_STAGE_COMPUTE_BIT:
			shaderType = Material::ShaderType::CS;
			break;
		default:
			break;
		}*/

		/*std::string text = "#version 450\n";
		std::set<std::string> defines = shaderDefines[shaderType];
		for (const std::string& define : defines)
		{
			text += define;
		}
		shader->insert(text);
		shader->compile();*/
	}
	return 0;
}

void MaterialVK::addConstantBuffer(std::string name, unsigned int location)
{
	m_UniformBuffer = new UniformBufferVK(m_Device, m_SwapChain);
}

void MaterialVK::updateConstantBuffer(const void* data, size_t size, unsigned int location)
{
	m_UniformBuffer->setBufferSize(size);
	m_UniformBuffer->setData(data, true);
}

int MaterialVK::enable()
{
	return 0;
}

void MaterialVK::disable()
{
}
