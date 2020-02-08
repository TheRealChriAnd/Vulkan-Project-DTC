#pragma once

class DescriptorSetVK;

class MeshVK
{
public:
	MeshVK();
	~MeshVK();

	DescriptorSetVK* m_DescriptorSet0;
	DescriptorSetVK* m_DescriptorSet1;
};