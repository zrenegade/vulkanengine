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

#ifndef controller_h
#define controller_h

#include <vulkan/vulkan.h>

#include "shared.h"

namespace vfsme
{

class Controller
{
public:
	Controller();
	~Controller();
	
	///@note Only define copy and move contructors and assignment operators if they are actually required
	Controller(const Controller&) = delete;
	Controller(Controller&&) = delete;
	Controller& operator=(const Controller&) = delete;
	Controller& operator=(Controller &&) = delete;

	void Init();
	void SetupQueue();
	void SetupDevice(const VkSurfaceKHR& surface);
	void Destroy();
	
	inline VkPhysicalDevice& GetPhysicalDevice() { return physicalDevice; }
	inline VkDevice& GetDevice() { return device; }
	inline VkInstance& GetInstance() { return instance; }
	
	void Configure(const VkSurfaceKHR& surface);
	bool PresentModeSupported(const VkSurfaceKHR& surface, VkPresentModeKHR presentMode);
	bool SurfaceFormatSupported(const VkSurfaceKHR& surface, VkFormat surfaceFormat) const;
	
	const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const { return memProperties; }
	
	//inline VkSurfaceCapabilitiesKHR* GetCapabilities() { return &capabilities; }
	inline uint32_t GetQueueFamilyId() const { return queueFamilyId; }
	inline uint32_t GetGraphicsQueueIndex() const { return graphicsQueueIndex; }
	inline uint32_t GetPresentQueueIndex() const { return presentQueueIndex; }
	inline uint32_t GetComputeQueueIndex() const { return computeQueueIndex; }
	
	void CheckFormatPropertyType(VkFormat format, VkFormatFeatureFlagBits flags) const;
	
private:
	void PrintCapabilities() const;

	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceFeatures deviceFeatures;
	VkInstance instance;
	VkDebugReportCallbackEXT callback;
	VkSurfaceCapabilitiesKHR capabilities;
	VkPhysicalDeviceMemoryProperties memProperties;
	float* queuePriorities;
	
	const uint32_t queueCount = 3;
	uint32_t queueFamilyId = InvalidIndex;
	const uint32_t graphicsQueueIndex = 0;
	const uint32_t presentQueueIndex = 1;
	const uint32_t computeQueueIndex = 2;
};

};

#endif