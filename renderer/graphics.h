#ifndef GRAPHICS_H
#define GRAPHICS_H
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "event.h"

#define MAXSHADERFILESIZE 2047 

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

typedef struct Window
{
  VulkanContext context;
  GLFWwindow * window;
  Event events[4];
  uint32_t numEvents;
} Window;

typedef struct WindowSize
{
  int width, height;
} WindowSize;

PFN_vkGetPhysicalDeviceQueueFamilyProperties pfnGetPhysicalDeviceQueueFamilyProperties;

void InitWindowEvents(Window * window);
void UpdateWindow(Window * window);
int ShouldClose(Window * window);
int CreateWindow(Window * window);
void DestroyWindow(Window * window);

PFN_vkCreateDevice pfnCreateDevice;
PFN_vkEnumeratePhysicalDevices pfnEnumeratePhysicalDevices;
PFN_vkGetPhysicalDeviceQueueFamilyProperties pfnGetPhysicalDeviceQueueFamilyProperties;
PFN_vkGetPhysicalDeviceFeatures pfnGetPhysicalDeviceFeatures;
PFN_vkCreateCommandPool pfnCreateCommandPool;
PFN_vkAllocateCommandBuffers pfnAllocateCommandBuffers;
PFN_vkResetCommandPool pfnResetCommandPool;
PFN_vkCreateRenderPass pfnCreateRenderPass;
PFN_vkCreateImage pfnCreateImage;
PFN_vkCreateImageView pfnCreateImageView;
PFN_vkCreateFramebuffer pfnCreateFramebuffer;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR pfnGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR pfnGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkCreateSwapchainKHR pfnCreateSwapchainKHR;
PFN_vkGetSwapchainImagesKHR pfnGetSwapchainImagesKHR;
PFN_vkCreatePipelineLayout pfnCreatePipelineLayout;
PFN_vkCreateGraphicsPipelines pfnCreateGraphicsPipelines;
PFN_vkCreateShaderModule pfnCreateShaderModule;
PFN_vkCreateSemaphore pfnCreateSemaphore;
PFN_vkAcquireNextImageKHR pfnAcquireNextImageKHR;
PFN_vkCmdBeginRenderPass pfnCmdBeginRenderPass;
PFN_vkCmdBindPipeline pfnCmdBindPipeline;
PFN_vkCmdDraw pfnCmdDraw;
PFN_vkCmdEndRenderPass pfnCmdEndRenderPass;
PFN_vkEndCommandBuffer pfnEndCommandBuffer;
PFN_vkGetDeviceQueue pfnGetDeviceQueue;
PFN_vkQueueSubmit pfnQueueSubmit;
PFN_vkQueuePresentKHR pfnQueuePresentKHR;
PFN_vkQueueWaitIdle pfnQueueWaitIdle;
PFN_vkBeginCommandBuffer pfnBeginCommandBuffer;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR pfnGetPhysicalDeviceSurfaceSupportKHR;
PFN_vkDestroySemaphore pfnDestroySemaphore;
PFN_vkDestroyCommandPool pfnDestroyCommandPool;
PFN_vkDestroyDevice pfnDestroyDevice;
PFN_vkDestroyInstance pfnDestroyInstance;
PFN_vkDestroyImageView pfnDestroyImageView;
PFN_vkDestroySwapchainKHR pfnDestroySwapchainKHR;
#endif
