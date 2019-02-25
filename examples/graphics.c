#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void WindowSizeCallback(GLFWwindow * window, int width, int height)
{
  WindowSize windowSize = { width, height};
  EventTrigger(&resizeEvent, (const void *)&windowSize);
}

#ifdef _DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
						    VkDebugUtilsMessageTypeFlagsEXT type,
						    const VkDebugUtilsMessengerCallbackDataEXT * callbackData,
						    void * userData)
{
  printf("Validation layer : %s \n", callbackData->pMessage);
  return 0;
}
#endif /// _DEBUG

#define FUNCIMPL(NAME) \
  NAME =  (PFN_##NAME )glfwGetInstanceProcAddress(instance, #NAME );

static int LoadVulkanFunctionPointers(VkInstance instance)
{
  pfnCreateDevice = (PFN_vkCreateDevice)glfwGetInstanceProcAddress(instance, "vkCreateDevice");
  pfnEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)glfwGetInstanceProcAddress(instance, "vkEnumeratePhysicalDevices");
  pfnGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)glfwGetInstanceProcAddress(instance, "vkGetPhysicalDeviceQueueFamilyProperties");
  pfnGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)glfwGetInstanceProcAddress(instance, "vkGetPhysicalDeviceFeatures");
  pfnCreateCommandPool = (PFN_vkCreateCommandPool)glfwGetInstanceProcAddress(instance, "vkCreateCommandPool");
  pfnResetCommandPool = (PFN_vkResetCommandPool)glfwGetInstanceProcAddress(instance, "vkResetCommandPool");

  pfnAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)glfwGetInstanceProcAddress(instance, "vkAllocateCommandBuffers");
  pfnCreateRenderPass = (PFN_vkCreateRenderPass)glfwGetInstanceProcAddress(instance,"vkCreateRenderPass");
  pfnCreateImage = (PFN_vkCreateImage)glfwGetInstanceProcAddress(instance,"vkCreateImage");
  pfnCreateImageView = (PFN_vkCreateImageView)glfwGetInstanceProcAddress(instance,"vkCreateImageView");
  pfnCreateFramebuffer = (PFN_vkCreateFramebuffer)glfwGetInstanceProcAddress(instance,"vkCreateFramebuffer");
  pfnGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)glfwGetInstanceProcAddress(instance,"vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
  pfnGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)glfwGetInstanceProcAddress(instance,"vkGetPhysicalDeviceSurfaceFormatsKHR");
  pfnCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)glfwGetInstanceProcAddress(instance,"vkCreateSwapchainKHR");
  pfnGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)glfwGetInstanceProcAddress(instance, "vkGetSwapchainImagesKHR");
  pfnCreatePipelineLayout = (PFN_vkCreatePipelineLayout)glfwGetInstanceProcAddress(instance, "vkCreatePipelineLayout");
  pfnCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)glfwGetInstanceProcAddress(instance, "vkCreateGraphicsPipelines");
  pfnCreateShaderModule = (PFN_vkCreateShaderModule)glfwGetInstanceProcAddress(instance, "vkCreateShaderModule");
  pfnCreateSemaphore = (PFN_vkCreateSemaphore)glfwGetInstanceProcAddress(instance, "vkCreateSemaphore");
  pfnAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)glfwGetInstanceProcAddress(instance, "vkAcquireNextImageKHR");
  pfnCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)glfwGetInstanceProcAddress(instance, "vkCmdBeginRenderPass");
  pfnCmdBindPipeline = (PFN_vkCmdBindPipeline)glfwGetInstanceProcAddress(instance, "vkCmdBindPipeline");
  pfnCmdDraw = (PFN_vkCmdDraw)glfwGetInstanceProcAddress(instance, "vkCmdDraw");
  pfnCmdEndRenderPass = (PFN_vkCmdEndRenderPass)glfwGetInstanceProcAddress(instance, "vkCmdEndRenderPass");
  pfnEndCommandBuffer = (PFN_vkEndCommandBuffer)glfwGetInstanceProcAddress(instance, "vkEndCommandBuffer");
  pfnGetDeviceQueue = (PFN_vkGetDeviceQueue)glfwGetInstanceProcAddress(instance, "vkGetDeviceQueue");
  pfnQueueSubmit = (PFN_vkQueueSubmit)glfwGetInstanceProcAddress(instance, "vkQueueSubmit");
  pfnQueuePresentKHR = (PFN_vkQueuePresentKHR)glfwGetInstanceProcAddress(instance, "vkQueuePresentKHR");
  pfnQueueWaitIdle = (PFN_vkQueueWaitIdle)glfwGetInstanceProcAddress(instance, "vkQueueWaitIdle");
  pfnBeginCommandBuffer = (PFN_vkBeginCommandBuffer)glfwGetInstanceProcAddress(instance, "vkBeginCommandBuffer");
  pfnGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)glfwGetInstanceProcAddress(instance, "vkGetPhysicalDeviceSurfaceSupportKHR");;
  pfnDestroySemaphore = (PFN_vkDestroySemaphore)glfwGetInstanceProcAddress(instance, "vkDestroySemaphore");
  pfnDestroyCommandPool = (PFN_vkDestroyCommandPool)glfwGetInstanceProcAddress(instance, "vkDestroyCommandPool");
  pfnDestroyDevice = (PFN_vkDestroyDevice)glfwGetInstanceProcAddress(instance, "vkDestroyDevice");
  pfnDestroyInstance = (PFN_vkDestroyInstance)glfwGetInstanceProcAddress(instance, "vkDestroyInstance");
  pfnDestroyImageView = (PFN_vkDestroyImageView)glfwGetInstanceProcAddress(instance, "vkDestroyImageView");
  pfnDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)glfwGetInstanceProcAddress(instance, "vkDestroySwapchainKHR");
  return 1;
}

int InitGraphics(VulkanContext * context)
{
  context->window = NULL;
  context->instance = VK_NULL_HANDLE;
  context->physicalDevice = VK_NULL_HANDLE;
  context->device = VK_NULL_HANDLE;
  context->commandPool = VK_NULL_HANDLE;
  context->presentationQueue = VK_NULL_HANDLE;
  context->graphicsQueue = VK_NULL_HANDLE;
  context->imageAvailable = VK_NULL_HANDLE;
  context->renderFinished = VK_NULL_HANDLE;
  context->surface = VK_NULL_HANDLE;
#ifdef _DEBUG
  context->debugMessenger = VK_NULL_HANDLE;
#endif //_DEBUG

  if (!glfwInit()) {
    printf("GLFW initialisation failed\n");
    return 0;
  }
  if (!glfwVulkanSupported()) {
    printf("Vulkan not supported\n");
    return 0;
  }
  else printf("Vulkan supported\n");

  { /// Create instance
    PFN_vkCreateInstance pfnCreateInstance =
      (PFN_vkCreateInstance)glfwGetInstanceProcAddress(NULL, "vkCreateInstance");
    if (!pfnCreateInstance) {
      printf("Unable to get vkCreateInstance\n");
      return 0;
    }
    uint32_t count;
    char *extensions[32];
    const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
    memcpy(extensions, glfwExtensions, count * sizeof(char*));
#ifdef _DEBUG
    extensions[count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    const char * layers[] = {"VK_LAYER_LUNARG_standard_validation"};
#endif
    const VkInstanceCreateInfo createInfo = {
                                    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				    NULL,
				    0,
				    NULL,
#ifdef _DEBUG
				    1,
				    layers,
#else
				    0,
				    NULL,
#endif
				    count,
				    (const char **)extensions
    };
    if (VK_SUCCESS != pfnCreateInstance(&createInfo, NULL, &(context->instance))) {
      printf("Failed to create instance\n");
      return 0;
    }
    else printf("Instance creation successful\n");
  } /// Create instance

#ifdef _DEBUG
  { /// Create debug messenger
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {
      VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      NULL,
      0,
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      debugCallback,
      NULL
    };
    PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)
      glfwGetInstanceProcAddress(context->instance, "vkCreateDebugUtilsMessengerEXT");

    pfnCreateDebugUtilsMessengerEXT(context->instance,
				    &debugUtilsMessengerCreateInfo,
				    NULL,
				    &(context->debugMessenger));
  } /// Create debug messenger
#endif /// ifdef _DEBUG

  if (!LoadVulkanFunctionPointers(context->instance))
    return 0;

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  context->window = glfwCreateWindow(400, 400, "Demo", NULL, NULL);
  if (context->window &&
      VK_SUCCESS == glfwCreateWindowSurface(context->instance,
					    context->window,
					    NULL,
					    &(context->surface)))
    printf("Window creation succeeded\n");
  else {
    printf("Failed to create window\n");
    return 0;
  }

  EventInit(&resizeEvent, "resize", sizeof(WindowSize));
  glfwSetWindowSizeCallback(context->window, &WindowSizeCallback);

  uint32_t presentationQueue = 0;
  uint32_t graphicsQueue = 0;
  { /// Find a physical device
    uint32_t nPhysicalDevices = 32;
    VkPhysicalDevice physicalDevices[32];
    if (VK_SUCCESS != pfnEnumeratePhysicalDevices(context->instance,
						  &nPhysicalDevices,
						  physicalDevices))
      printf("Unsuccessful enumeratePhysicalDevices call");
    else printf("Num devices: %u\n", nPhysicalDevices);
  
    for (uint32_t i = 0; i < nPhysicalDevices; ++i) {
      uint32_t queueFamilyCount = 16;
      VkQueueFamilyProperties queueFamilyProperties[16];
      pfnGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, queueFamilyProperties);
      int hasPresentationQueue = 0;
      int hasGraphicsQueue = 0;
      for (uint32_t j = 0; j < queueFamilyCount; ++j) {
        VkBool32 support;
        pfnGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[i],
					      j,
					      context->surface,
					      &support);
        if (glfwGetPhysicalDevicePresentationSupport(context->instance, physicalDevices[i], j) && support) {
          printf("Supports Graphics Presentation\n");
          presentationQueue = j;
          hasPresentationQueue = 1;
        }
        if (queueFamilyProperties[j].queueCount > 0 && queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
          graphicsQueue = j;
          hasGraphicsQueue = 1;
        }
        if (hasPresentationQueue && hasGraphicsQueue){
          context->physicalDevice = physicalDevices[i];
          break;
        }
      }
    }
    if (context->physicalDevice == VK_NULL_HANDLE) {
      printf("No suitable physical devices found");
      return 0;
    }
  } /// Find a physical device
  { /// Create a logical device
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    pfnGetPhysicalDeviceFeatures(context->physicalDevice, &physicalDeviceFeatures);

    const float priority = 0.9f;
    uint32_t nDeviceQueueCreateInfos = 2;
    if (presentationQueue == graphicsQueue)
      nDeviceQueueCreateInfos = 1;
    const VkDeviceQueueCreateInfo deviceQueueCreateInfo[] = {
      {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        NULL,
        0,
        presentationQueue,
        1,
        &priority
      },
      {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        NULL,
        0,
        graphicsQueue,
        1,
        &priority
      }
    };
    const char * extensionNames[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    const VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
					    NULL,
					    0,
					    nDeviceQueueCreateInfos,
					    deviceQueueCreateInfo,
					    0,
					    NULL,
					    1,
					    extensionNames,
					    &physicalDeviceFeatures};

    if (VK_SUCCESS == pfnCreateDevice(context->physicalDevice,
				      &deviceCreateInfo,
				      NULL,
				      &(context->device)))
      printf("Logical device creation succeeded\n");
  } /// Create a logical device

  pfnGetDeviceQueue(context->device, presentationQueue, 0, &(context->presentationQueue));
  pfnGetDeviceQueue(context->device, graphicsQueue, 0, &(context->graphicsQueue));


  const VkCommandPoolCreateInfo commandPoolCreateInfo = {
    VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    NULL,
    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    graphicsQueue
  };

  if (VK_SUCCESS == pfnCreateCommandPool(context->device,
					 &commandPoolCreateInfo,
					 NULL,
					 &(context->commandPool)))
      printf("Command pool creation succeeded\n");
  else {
    printf("Command pool creation failed\n");
    return 0;
  }


  const VkSemaphoreCreateInfo semaphoreCreateInfo = {
    VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    NULL,
    0
  };
  if (VK_SUCCESS != pfnCreateSemaphore(context->device,
				       &semaphoreCreateInfo,
				       NULL,
				       &(context->imageAvailable))) {
    printf("Failed to create semaphore\n");
    return 0;
  } else printf("Semaphore creation succeeded\n");

  if (VK_SUCCESS != pfnCreateSemaphore(context->device,
				       &semaphoreCreateInfo,
				       NULL,
				       &(context->renderFinished))) {
    printf("Failed to create semaphore\n");
    return 0;
  } else printf("Semaphore creation succeeded\n");

  return 1;
}


void FreeGraphics(VulkanContext * context)
{
    
  if (context->instance != VK_NULL_HANDLE) {
    if (context->device != VK_NULL_HANDLE) {
      if (context->renderFinished != VK_NULL_HANDLE)
        pfnDestroySemaphore(context->device, context->renderFinished, NULL);
      if (context->imageAvailable != VK_NULL_HANDLE)
        pfnDestroySemaphore(context->device, context->imageAvailable, NULL);
      if (context->commandPool != VK_NULL_HANDLE)
	pfnDestroyCommandPool(context->device, context->commandPool, NULL);
      pfnDestroyDevice(context->device, NULL);
    }
    if (context->debugMessenger != VK_NULL_HANDLE) {
      PFN_vkDestroyDebugUtilsMessengerEXT pfnDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)
        glfwGetInstanceProcAddress(context->instance,
			         "vkDestroyDebugUtilsMessengerEXT");
      if(pfnDestroyDebugUtilsMessengerEXT)
        pfnDestroyDebugUtilsMessengerEXT(context->instance,
				         context->debugMessenger,
				         NULL);
    }
    pfnDestroyInstance(context->instance, NULL);
  }
  glfwDestroyWindow(context->window);
  glfwTerminate();
}
