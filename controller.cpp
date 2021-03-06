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

#include "controller.h"

#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <limits>
#include <stdexcept>

namespace vfsme
{

const char* layers[] = {
	"VK_LAYER_LUNARG_standard_validation",
	"VK_LAYER_RENDERDOC_Capture"
};

const uint32_t numLayers = 2;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData)
{
    std::cerr << msg << std::endl;

    return VK_FALSE;
}

Controller::Controller()
{
	queuePriorities = new float[queueCount]();
}

Controller::~Controller()
{
	delete[] queuePriorities;
}

void Controller::Init()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
		
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	VkExtensionProperties* extensions = new VkExtensionProperties[extensionCount]();
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);
	
	for(uint32_t i = 0; i < extensionCount; ++i)
	{
		std::cout << extensions[i].extensionName << std::endl;
	}
	
	uint32_t requestedExtensionCount = 3;
	
	const char* requestedExtensions[] =
	{
		"VK_KHR_surface",
		"VK_KHR_win32_surface",
		"VK_EXT_debug_report"
	};
	
	bool supported = false;
	
	for(uint32_t i = 0; i < requestedExtensionCount; ++i)
	{
		supported = false;
	
		for(uint32_t j = 0; j < extensionCount; ++j)
		{
			if (strncmp(requestedExtensions[i], extensions[j].extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0)
			{
				supported = true;
			}
		}
		
		if (supported == false)
		{
			std::cout << "Extension " << requestedExtensions[i] << " not supported" << std::endl;
			
			break;
		}
	}
	
	if (supported == false)
	{
		throw std::runtime_error("Extension not supported");
	}
		
	createInfo.enabledExtensionCount = requestedExtensionCount;
	createInfo.ppEnabledExtensionNames = requestedExtensions;
	
	uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    VkLayerProperties* availableLayers = new VkLayerProperties[layerCount]();; 
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
		
	supported = false;
	
	for(uint32_t i = 0; i < numLayers; ++i)
	{
		for(uint32_t j = 0; j < layerCount; ++j)
		{
			if (strcmp(layers[i], availableLayers[j].layerName) == 0)
			{
				supported = true;
				break;
			}
			
			std::cout << availableLayers[j].layerName << std::endl;
		}
		
		if (supported == false)
		{
			std::cout << "Layer not supported:" << layers[i] << std::endl;
		}
	}
	
	if (supported == false)
	{
		throw std::runtime_error("Layer not supported");
	}
	
	createInfo.enabledLayerCount = 1;
    createInfo.ppEnabledLayerNames = layers;
	
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create instance");
	}	
	
	VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
	callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	callbackCreateInfo.pfnCallback = debugCallback;
	
	auto vkCreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	
	VkResult callbackResult = vkCreateDebugReportCallback(instance, &callbackCreateInfo, nullptr, &callback);
	
	if (callbackResult != VK_SUCCESS)
	{
		throw std::runtime_error("Callback not created succesfuly");
	}
	
	physicalDevice = VK_NULL_HANDLE;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	
	if (deviceCount == 0)
	{
		throw std::runtime_error("Failed to find a device that supports Vulkan");	
	}
	
	VkPhysicalDevice* devices = new VkPhysicalDevice[deviceCount]();
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
	VkPhysicalDeviceProperties deviceProperties;
	
	bool foundDiscreteGPU = false;
	
	std::cout << "Device count: " << deviceCount << std::endl;
	
	for (uint32_t i = 0; i < deviceCount; ++i)
	{
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		
		std::cout << "Device type: " << deviceProperties.deviceType << std::endl;
		
		std::cout << "Storage buffer offset alignment: " << deviceProperties.limits.minStorageBufferOffsetAlignment << std::endl;
		
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			physicalDevice = devices[i];
			foundDiscreteGPU = true;
			break;
		}
	}
	
	if (foundDiscreteGPU == false)
	{
		throw std::runtime_error("failed to find a discrete GPU");
	}

	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
	
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	delete[] devices;
	delete[] availableLayers;
	delete[] extensions;
	
	CheckFormatPropertyType(VK_FORMAT_R32_SFLOAT, VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT);
}

void Controller::Destroy()
{
	auto vkDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    vkDestroyDebugReportCallback(instance, callback, nullptr);
	
	vkDestroyInstance(instance, nullptr);
}

void Controller::SetupQueue()
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	
	if (queueFamilyCount == 0)
	{
		throw std::runtime_error("Failed to find a queue that supports the device");	
	}
	
	VkQueueFamilyProperties* queueFamilies = new VkQueueFamilyProperties[queueFamilyCount]();
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);
	
	for (uint32_t i = 0; i < queueFamilyCount; ++i)
	{
		std::cout << "queue families: " << queueFamilies[i].queueCount << ", " << std::hex << queueFamilies[i].queueFlags << std::dec << std::endl;
		
		if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			queueFamilyId = i;
			break;
		}
	}
	
	if (queueFamilyId == InvalidIndex)
	{
		throw std::runtime_error("Failed to get queue family index");;
	}
	
	delete[] queueFamilies;
}

void Controller::SetupDevice(const VkSurfaceKHR& surface)
{
	VkBool32 presentSupport = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyId, surface, &presentSupport);
	
	if (presentSupport != true)
	{
		throw std::runtime_error("surface presetation not supported");
	}

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	
	queuePriorities[graphicsQueueIndex] = 1.0f;
	queuePriorities[presentQueueIndex] = 1.0f; 
	
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = queueFamilyId;
	queueCreateInfo.queueCount = queueCount;
	queueCreateInfo.pQueuePriorities = queuePriorities;
	
	uint32_t deviceExtensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);

    VkExtensionProperties* availableDeviceExtensions = new VkExtensionProperties[deviceExtensionCount]();
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, availableDeviceExtensions);
	
	uint32_t requestedDeviceExtensionCount = 1;
	
	const char* requestedDeviceExtension = "VK_KHR_swapchain";
	
	bool supported = false;
	
	for(uint32_t i = 0; i < deviceExtensionCount; ++i)
	{
		std::cout << availableDeviceExtensions[i].extensionName << std::endl;
		
		if (strncmp(requestedDeviceExtension, availableDeviceExtensions[i].extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0)
		{
			supported = true;
		}
	}
	
	if (supported == false)
	{
		throw std::runtime_error("VK_KHR_swapchain device extension not supported");
	}
	
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledLayerCount = 1;
    deviceCreateInfo.ppEnabledLayerNames = layers;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.enabledExtensionCount = requestedDeviceExtensionCount;
	deviceCreateInfo.ppEnabledExtensionNames = &requestedDeviceExtension;
	
	VkResult deviceResult = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
	
	if (deviceResult != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create device");
	}

	delete[] availableDeviceExtensions;
}

void Controller::Configure(const VkSurfaceKHR& surface)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
	
	PrintCapabilities();
	
	VkSurfaceTransformFlagBitsKHR transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	
	if ((capabilities.supportedTransforms & transform) == 0)
	{
		throw std::runtime_error("Transform not supported");
	}
}

void Controller::PrintCapabilities() const
{
	/*uint32_t minImageCount;
    uint32_t maxImageCount;
    VkExtent2D currentExtent;
    VkExtent2D minImageExtent;
    VkExtent2D maxImageExtent;
    uint32_t maxImageArrayLayers;
    VkSurfaceTransformFlagsKHR supportedTransforms;
    VkSurfaceTransformFlagBitsKHR currentTransform;
    VkCompositeAlphaFlagsKHR supportedCompositeAlpha;
    VkImageUsageFlags supportedUsageFlags;*/
	
	std::cout << capabilities.minImageCount << std::endl;
	std::cout << capabilities.maxImageCount << std::endl;
	std::cout << capabilities.currentExtent.width << std::endl;
	std::cout << capabilities.currentExtent.height << std::endl;
	std::cout << capabilities.minImageExtent.width << std::endl;
	std::cout << capabilities.minImageExtent.height << std::endl;
	std::cout << capabilities.maxImageExtent.width << std::endl;
	std::cout << capabilities.maxImageExtent.height << std::endl;
	std::cout << capabilities.maxImageArrayLayers << std::endl;
	std::cout << capabilities.supportedTransforms << std::endl;
	std::cout << capabilities.currentTransform << std::endl;
	std::cout << capabilities.supportedCompositeAlpha << std::endl;
	std::cout << capabilities.supportedUsageFlags << std::endl;
}

bool Controller::PresentModeSupported(const VkSurfaceKHR& surface, VkPresentModeKHR presentMode)
{
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
	VkPresentModeKHR* supportedPresentModes = new VkPresentModeKHR[presentModeCount]();
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, supportedPresentModes);
	
	bool supported = false;	
	
	for(uint32_t i = 0; i < presentModeCount; ++i)
	{
		std::cout << std::hex << supportedPresentModes[i] << std::endl;
		
		if (supportedPresentModes[i] == presentMode)
		{
			supported = true;
		}
	}
	
	if (!supported)
	{
		std::cout << "Present mode not supported" << std::endl;
	}
	
	delete[] supportedPresentModes;
	
	return supported;
}

bool Controller::SurfaceFormatSupported(const VkSurfaceKHR& surface, VkFormat surfaceFormat) const
{
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[formatCount]();
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats);
		
	bool supported = false;
	
	std::cout << std::endl;
	for(uint32_t i = 0; i < formatCount; ++i)
	{
		std::cout << std::hex << surfaceFormats[i].format << ", " << surfaceFormats[i].colorSpace << std::endl;
	
		if (surfaceFormats[i].format == surfaceFormat)
		{
			supported = true;
		}
	}
	
	if (!supported)
	{
		std::cout << "Format not supported" << std::endl;
	}
	
	delete[] surfaceFormats;
	
	return supported;
}

void Controller::CheckFormatPropertyType(VkFormat format, VkFormatFeatureFlagBits flags) const
{
	VkFormatProperties formatProps;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
	
	if ((formatProps.optimalTilingFeatures & flags) == 0)
	{
		throw std::runtime_error("Format property not optimal");
	}
}

};