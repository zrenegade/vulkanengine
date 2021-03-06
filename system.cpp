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

#include "system.h"

#include <iostream>

namespace vfsme
{

void System::Init(uint32_t width, uint32_t height)
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, "Vulkan window", nullptr, nullptr);

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
}

void System::Destroy()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void System::Loop(Compositor& composer, VkDevice& device)
{
	while(!glfwWindowShouldClose(window))
	{
        glfwPollEvents();
		
		composer.Draw(device);
    }
}

void System::CreateSurface(VkInstance& instance, VkSurfaceKHR* surface)
{
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hwnd = glfwGetWin32Window(window);
	surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	
	auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
	
	VkResult result = CreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, surface);
	//glfwCreateWindowSurface(instance, window, nullptr, &surface)
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create surface");;
	}
}

void System::DestroySurface(VkInstance& instance, VkSurfaceKHR& surface)
{
	auto DestroySurfaceKHR = (PFN_vkDestroySurfaceKHR) vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR");
	DestroySurfaceKHR(instance, surface, nullptr);
}

bool System::CheckExtensionsSupport(uint32_t extensionCount, const VkExtensionProperties* extensions) const
{
	bool fullySupported = true;
	
	for(unsigned int i = 0; i < glfwExtensionCount; ++i)
	{
		std::cout << glfwExtensions[i] << std::endl;
		
		bool extensionSupported = false;
	
		for(unsigned int j = 0; j < extensionCount; ++j)
		{
			if (strcmp(glfwExtensions[i], extensions[j].extensionName) == 0)
			{
				extensionSupported = true;
				break;
			}
		}
		
		if (extensionSupported == false)
		{
			std::cout << "Extension not supported:" << glfwExtensions[i] << std::endl;
			fullySupported = false;
		}
	}
	
	return fullySupported;
}

};