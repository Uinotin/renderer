#include "vulkancontext.h"


#define FUNCTOSTRING(NAME) \
  #NAME
#define FUNCIMPL(NAME) \
  pfn##NAME = (PFN_vk##NAME )glfwGetInstanceProcAddress(instance, FUNCTOSTRING(vk##NAME) )

int LoadVulkanFunctionPointers(VkInstance instance)
{
  FUNCIMPL(CreateDevice);
  FUNCIMPL(EnumeratePhysicalDevices);
  FUNCIMPL(GetPhysicalDeviceQueueFamilyProperties);
  FUNCIMPL(GetPhysicalDeviceFeatures);
  FUNCIMPL(CreateCommandPool);
  FUNCIMPL(ResetCommandPool);
  FUNCIMPL(AllocateCommandBuffers);
  FUNCIMPL(CreateRenderPass);
  FUNCIMPL(CreateImage);
  FUNCIMPL(CreateImageView);
  FUNCIMPL(CreateFramebuffer);
  FUNCIMPL(GetPhysicalDeviceSurfaceCapabilitiesKHR);
  FUNCIMPL(GetPhysicalDeviceSurfaceFormatsKHR);
  FUNCIMPL(GetImageMemoryRequirements);
  FUNCIMPL(GetPhysicalDeviceMemoryProperties);
  FUNCIMPL(CreateSwapchainKHR);
  FUNCIMPL(GetSwapchainImagesKHR);
  FUNCIMPL(CreatePipelineLayout);
  FUNCIMPL(CreateGraphicsPipelines);
  FUNCIMPL(CreateShaderModule);
  FUNCIMPL(CreateSemaphore);
  FUNCIMPL(AcquireNextImageKHR);
  FUNCIMPL(CmdBeginRenderPass);
  FUNCIMPL(CmdBindPipeline);
  FUNCIMPL(CmdDraw);
  FUNCIMPL(CmdEndRenderPass);
  FUNCIMPL(CmdPushConstants);
  FUNCIMPL(CmdPipelineBarrier);
  FUNCIMPL(EndCommandBuffer);
  FUNCIMPL(GetDeviceQueue);
  FUNCIMPL(QueueSubmit);
  FUNCIMPL(QueuePresentKHR);
  FUNCIMPL(QueueWaitIdle);
  FUNCIMPL(BeginCommandBuffer);
  FUNCIMPL(GetPhysicalDeviceSurfaceSupportKHR);
  FUNCIMPL(DestroySemaphore);
  FUNCIMPL(DestroyCommandPool);
  FUNCIMPL(DestroyDevice);
  FUNCIMPL(DestroyInstance);
  FUNCIMPL(DestroyImageView);
  FUNCIMPL(DestroySwapchainKHR);
  FUNCIMPL(FreeCommandBuffers);
  FUNCIMPL(FreeMemory);
  FUNCIMPL(DestroyRenderPass);
  FUNCIMPL(DestroyFramebuffer);
  FUNCIMPL(DestroyPipelineLayout);
  FUNCIMPL(DestroyPipeline);
  FUNCIMPL(DestroyShaderModule);
  FUNCIMPL(DestroyImage);
  FUNCIMPL(DeviceWaitIdle);
  FUNCIMPL(AllocateMemory);
  FUNCIMPL(BindImageMemory);
  return 1;
}
