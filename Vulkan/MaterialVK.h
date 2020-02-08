#pragma once

#include <vector>
/*
 * extend this class with a concrete implementation,
 * public interface should not change...
 *
 * A material represents the programmable part of the pipeline:
 * Vertex, Geometry (wont be used), Fragment and Compute Shaders (wont be used)
 * Only need to support Vertex and Fragment for now.
 * Any extra functionality should be added to a concrete subclass
 */

class DeviceVK;
class ShaderVK;
class UniformBufferVK;
class SwapChainVK;

class MaterialVK
{
public:

	MaterialVK(DeviceVK* device, SwapChainVK* swapChain, const std::string& name);
	virtual ~MaterialVK();

	const std::vector<ShaderVK*>& getShaders() const;

	UniformBufferVK* getUniformBuffer() const;

	// set shader name, DOES NOT COMPILE
	virtual void setShader(const std::string& shaderFileName);

	/*
	 * Compile and link all shaders
	 * Returns 0  if compilation/linking succeeded.
	 * Returns -1 if compilation/linking fails.
	 * Error is returned in errString
	 * A Vertex and a Fragment shader MUST be defined.
	 * If compileMaterial is called again, it should RE-COMPILE the shader
	 * In principle, it should only be necessary to re-compile if the defines set
	 * has changed.
	*/
	virtual int compileMaterial(std::string& errString);

	// this constant buffer will be bound every time we bind the material
	virtual void addConstantBuffer(std::string name, unsigned int location);

	// location identifies the constant buffer in a unique way
	virtual void updateConstantBuffer(const void* data, size_t size, unsigned int location);

	// activate the material for use.
	virtual int enable();

	// disable material
	virtual void disable();

private:
	UniformBufferVK* m_UniformBuffer;
	std::vector<ShaderVK*> m_Shaders;
	std::string m_Name;

	DeviceVK* m_Device;
	SwapChainVK* m_SwapChain;
};