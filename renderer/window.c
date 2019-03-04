#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Window * windows[1];
uint32_t numWindows;

WasdKeyStatus wasd;
static void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
  for (uint32_t i = 0; i < numWindows; ++i)
    if(windows[i]->window == window) {
      /// HACK, make this more generic later
      if ((action == GLFW_RELEASE || action == GLFW_PRESS) &&
	  (key == GLFW_KEY_W || key == GLFW_KEY_A || key == GLFW_KEY_S || key == GLFW_KEY_D ||
	   key == GLFW_KEY_UP || key == GLFW_KEY_DOWN ||
	   key == GLFW_KEY_RIGHT || key == GLFW_KEY_LEFT)) {
        switch (key) {
        case GLFW_KEY_W:
          wasd.w = action == GLFW_RELEASE ? 0 : 1;
          break;
        case GLFW_KEY_A:
          wasd.a = action == GLFW_RELEASE ? 0 : 1;
          break;
        case GLFW_KEY_S:
          wasd.s = action == GLFW_RELEASE ? 0 : 1;
          break;
        case GLFW_KEY_D:
          wasd.d = action == GLFW_RELEASE ? 0 : 1;
          break;
        case GLFW_KEY_UP:
          wasd.up = action == GLFW_RELEASE ? 0 : 1;
          break;
        case GLFW_KEY_DOWN:
          wasd.down = action == GLFW_RELEASE ? 0 : 1;
          break;
        case GLFW_KEY_LEFT:
          wasd.left = action == GLFW_RELEASE ? 0 : 1;
          break;
        case GLFW_KEY_RIGHT:
          wasd.right = action == GLFW_RELEASE ? 0 : 1;
          break;
        default:
          break;
        }
	EventTrigger(&(windows[i]->events[4]), &wasd);
      }
      break;
    }
}

static void WindowSizeCallback(GLFWwindow * window, int width, int height)
{
  WindowSize windowSize = { width, height};

  for (uint32_t i = 0; i < numWindows; ++i)
    if(windows[i]->window == window) {
      EventTrigger(windows[i]->events + 1, (const void *)&windowSize);
      break;
    }
}

static void WindowCloseCallback(GLFWwindow * window)
{
  int closing = 1;
  for (uint32_t i = 0; i < numWindows; ++i)
    if(windows[i]->window == window) {
      EventTrigger(windows[0]->events, (const void *)&closing);
      break;
    }
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

void InitWindowEvents(Window * window)
{
  window->numEvents = 5;
  EventInit(&(window->events[0]), "exit", sizeof(int));
  EventInit(&(window->events[1]), "resize", sizeof(WindowSize));
  EventInit(&(window->events[2]), "context", sizeof(VulkanContext));
  EventInit(&(window->events[3]), "update", sizeof(double));
  EventInit(&(window->events[4]), "wasd", sizeof(WasdKeyStatus));
}
double prevTime;
void UpdateWindow(Window * window)
{
  glfwPollEvents();
  double timeNow = glfwGetTime();
  double dt = timeNow - prevTime;
  prevTime = timeNow;
  if (dt < 1.0 / 30.0) { 
    usleep((useconds_t)((1.0/30-dt)/0.000001));
    timeNow = glfwGetTime();
    dt += timeNow - prevTime;
    prevTime = timeNow;
  }
  EventTrigger(window->events + 3, &dt);
}

int ShouldClose(Window * window)
{
  return glfwWindowShouldClose(window->window);
}

int CreateWindow(Window * window)
{
  /// As we won't be having multiple windows anytime soon,
  /// let's assume we have only one.
  windows[0] = window;
  numWindows = 1;
  VulkanContext * context = &(window->context);
  window->window = NULL;
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
  prevTime = glfwGetTime();
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
  window->window = glfwCreateWindow(400, 400, "Demo", NULL, NULL);
  if (window->window &&
      VK_SUCCESS == glfwCreateWindowSurface(context->instance,
					    window->window,
					    NULL,
					    &(context->surface)))
    printf("Window creation succeeded\n");
  else {
    printf("Failed to create window\n");
    return 0;
  }

  glfwSetKeyCallback(window->window, &KeyCallback);
  glfwSetWindowSizeCallback(window->window, &WindowSizeCallback);
  glfwSetWindowCloseCallback(window->window, &WindowCloseCallback);

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

  {
    int closing = 0;
    EventTrigger(window->events, &closing);
    EventTrigger(window->events + 2, context);
    WindowSize windowSize = {400, 400};
    EventTrigger(window->events + 1, &windowSize);
  }
  return 1;
}


void DestroyWindow(Window * window)
{
  VulkanContext * context = &(window->context);
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
    #ifdef _DEBUG
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
#endif /// _DEBUG
    pfnDestroyInstance(context->instance, NULL);
  }
  glfwDestroyWindow(window->window);
  glfwTerminate();
}
