#ifndef controller_h
#define controller_h

#include <vulkan/vulkan.h>

const uint32_t InvalidIndex = 0xffffffff;

class Controller
{
public:
	bool Init();
	bool SetupQueue();
	bool SetupDevice(VkSurfaceKHR& surface);
	bool Destroy();
	
	VkPhysicalDevice& GetPhysicalDevice() { return physicalDevice; }
	VkDevice& GetDevice() { return device; }
	VkInstance& GetInstance() { return instance; }
	bool Configure(VkSurfaceKHR& surface);
	bool PresentModeSupported(VkPresentModeKHR& presentMode, VkSurfaceKHR& surface);
	bool SurfaceFormatSupported(VkSurfaceKHR& surface, VkFormat& surfaceFormat);
	
	//VkSurfaceCapabilitiesKHR* GetCapabilities() { return &capabilities; }
	//uint32_t GetQueueFamilyId() { return queueFamilyId; }
	//const char* GetLayer() { return layer; }
	
	uint32_t GetQueueFamilyId() { return queueFamilyId; }
	uint32_t GetGraphicsQueueIndex() { return graphicsQueueIndex; }
	uint32_t GetPresentQueueIndex(){ return presentQueueIndex; }
	
	Controller();
	~Controller();
	
private:
	void PrintCapabilities();

	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceFeatures deviceFeatures;
	VkInstance instance;
	VkDebugReportCallbackEXT callback;
	VkSurfaceCapabilitiesKHR capabilities;
	uint32_t queueFamilyId = InvalidIndex;
	const char* layer = "VK_LAYER_LUNARG_standard_validation";
	const uint32_t graphicsQueueIndex = 0;
	const uint32_t presentQueueIndex = 1;
};

#endif