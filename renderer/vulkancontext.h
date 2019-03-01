#ifndef VULKANCONTEXT_H
#define VULKANCONTEXT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

typedef struct VulkanContext
{
  VkInstance instance;
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkCommandPool commandPool;
  VkQueue presentationQueue;
  VkQueue graphicsQueue;
  VkSemaphore imageAvailable;
  VkSemaphore renderFinished;
  VkSurfaceKHR surface;
#ifdef _DEBUG
  VkDebugUtilsMessengerEXT debugMessenger;
#endif //_DEBUG
} VulkanContext;

int LoadVulkanFunctionPointers(VkInstance instance);

#define FUNCDECL(NAME) \
  PFN_vk##NAME pfn##NAME
FUNCDECL(GetPhysicalDeviceQueueFamilyProperties);
FUNCDECL(CreateDevice);
FUNCDECL(EnumeratePhysicalDevices);
FUNCDECL(GetPhysicalDeviceQueueFamilyProperties);
FUNCDECL(GetPhysicalDeviceFeatures);
FUNCDECL(CreateCommandPool);
FUNCDECL(AllocateCommandBuffers);
FUNCDECL(ResetCommandPool);
FUNCDECL(CreateRenderPass);
FUNCDECL(CreateImage);
FUNCDECL(CreateImageView);
FUNCDECL(CreateFramebuffer);
FUNCDECL(GetPhysicalDeviceSurfaceCapabilitiesKHR);
FUNCDECL(GetPhysicalDeviceSurfaceFormatsKHR);
FUNCDECL(CreateSwapchainKHR);
FUNCDECL(GetSwapchainImagesKHR);
FUNCDECL(CreatePipelineLayout);
FUNCDECL(CreateGraphicsPipelines);
FUNCDECL(CreateShaderModule);
FUNCDECL(CreateSemaphore);
FUNCDECL(AcquireNextImageKHR);
FUNCDECL(CmdBeginRenderPass);
FUNCDECL(CmdBindPipeline);
FUNCDECL(CmdDraw);
FUNCDECL(CmdEndRenderPass);
FUNCDECL(CmdPushConstants);
FUNCDECL(EndCommandBuffer);
FUNCDECL(GetDeviceQueue);
FUNCDECL(QueueSubmit);
FUNCDECL(QueuePresentKHR);
FUNCDECL(QueueWaitIdle);
FUNCDECL(BeginCommandBuffer);
FUNCDECL(GetPhysicalDeviceSurfaceSupportKHR);
FUNCDECL(DestroySemaphore);
FUNCDECL(DestroyCommandPool);
FUNCDECL(DestroyDevice);
FUNCDECL(DestroyInstance);
FUNCDECL(DestroyImageView);
FUNCDECL(DestroySwapchainKHR);
FUNCDECL(FreeCommandBuffers);
FUNCDECL(DestroyRenderPass);
FUNCDECL(DestroyFramebuffer);
FUNCDECL(DestroyPipelineLayout);
FUNCDECL(DestroyPipeline);
FUNCDECL(DestroyShaderModule);
FUNCDECL(DeviceWaitIdle);

#endif /// VULKANCONTEXT_H
