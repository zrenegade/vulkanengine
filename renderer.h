/**
 * Copyright (C) 2016 Nigel Williams
 *
 * Vulkan Free Surface Modeling Engine (VFSME) is free software:
 * you can redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef renderer_h
#define renderer_h

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <functional>

#include "commands.h"

namespace vfsme
{

class Renderer : Commands
{
public:
	Renderer(const VkExtent2D& screenExtent, const VkExtent3D& gridDim, const VkPhysicalDeviceMemoryProperties& memProps);
	~Renderer();
	
	///@note Only define copy and move contructors and assignment operators if they are actually required
    Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer &&) = delete;
	
	void Init(VkDevice& device, const VkFormat& surfaceFormat, const VkImageView* imageViews, uint32_t queueFamilyId);
	void Destroy(VkDevice& device);
	
	void ConstructFrames(const VkBuffer& heightBuffer, const VkBuffer& normalBuffer);
	
	inline VkCommandBuffer* GetFrame(uint32_t index) const { return &drawCommandBuffers[index]; }
	
	VkCommandBuffer& TransferStaticBuffers(VkDevice& device);
	VkCommandBuffer& TransferDynamicBuffers(VkDevice& device);
	
private:
	void SetupIndexBuffer(VkDevice& device);
	void SetupClientSideVertexBuffer(VkDevice& device);
	void SetupServerSideVertexBuffer(VkDevice& device);
	void SetupUniformBuffer(VkDevice &device);
	void SetupDynamicTransfer(VkDevice &device);
	void SetupStaticTransfer(VkDevice &device);	
	void SetupShaderParameters(VkDevice& device);

	VkExtent2D imageExtent;
	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkPipeline pipeline;
	VkFramebuffer* framebuffers;
	VkCommandPool commandPool;
	VkCommandBuffer* drawCommandBuffers;
	VkCommandBuffer staticTransferCommandBuffer;
	VkCommandBuffer dynamicTransferCommandBuffer;
	
	VkBuffer vertexBuffer;
	VkBuffer vertexTransferBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory vertexTransferBufferMemory;
	
	VkBuffer indexTransferBuffer;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkDeviceMemory indexTransferBufferMemory;
	
	VkBuffer uniformTransferBuffer;
	VkDeviceMemory uniformTransferBufferMemory;
	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;
	
	VkBuffer* heightBuffer;

	VkVertexInputAttributeDescription* attributeDescriptions;
	VkVertexInputBindingDescription* bindingDescriptions;
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;
	
	const VkExtent3D grid;
	
	const uint32_t numFBOs = 2;
	const uint32_t numDrawCmdBuffers = 2;
	const uint32_t numAttrDesc = 4;
	const uint32_t numBindDesc = 3;
	const uint32_t numComponents = 3;
	const uint32_t numVertexElements = 2;
	
	uint32_t mat4Size;
	uint32_t uboSize;
	
	float* vertexInfo;
	uint16_t* indices;
	
	uint32_t vertexInfoSize;
	uint32_t numIndices;
	uint32_t indicesBufferSize;
	uint32_t numVerts;
	uint32_t numPrims;
};

};

#endif