#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PFN_vkCreateInstance pfnCreateInstance;
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
VkRenderPass createRenderPass(VkDevice device, const VkAllocationCallbacks * allocator);
VkInstance getFunctionPointers(void);
void * allocator(void *, size_t size, size_t alignment, VkSystemAllocationScope);
void * reallocator(void *, void * original, size_t size, size_t, VkSystemAllocationScope);
void freeFunc(void *, void *pointer);

void * allocator(void *user, size_t size, size_t alignment, VkSystemAllocationScope scope)
{
  return malloc(size);
}

void * reallocator(void *user, void * original, size_t size, size_t alignment, VkSystemAllocationScope scope)
{
  return realloc(original, size);
}

void freeFunc(void *user, void *pointer)
{
  free(pointer);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
						    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
						    VkDebugUtilsMessageTypeFlagsEXT type,
						    const VkDebugUtilsMessengerCallbackDataEXT * callbackData,
						    void * userData)
{
  printf("Validation layer : %s \n", callbackData->pMessage);
  return 0;
}

VkInstance getFunctionPointers(void)
{
  pfnCreateInstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(NULL, "vkCreateInstance");
  if (!pfnCreateInstance) {
    printf("Unable to get vkCreateInstance\n");
    return 0;
  }
  uint32_t count;
  char *extensions[32];
  const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
  memcpy(extensions, glfwExtensions, count * sizeof(char*));
  extensions[count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
  const char * layers[] = {"VK_LAYER_LUNARG_standard_validation"};
  const VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				  NULL,
				  0,
				  NULL,
				  1,
				  layers,
				  count,
				  (const char **)extensions};
  const VkAllocationCallbacks allocationCallbacks = {NULL, allocator, reallocator, freeFunc, NULL, NULL};
  VkInstance instance;
  if (VK_SUCCESS != pfnCreateInstance(&createInfo, &allocationCallbacks, &instance)) {
    printf("Failed to create instance\n");
    return 0;
  }
  else printf("Instance creation successful\n");

  VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {
    VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    NULL,
    0,
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    debugCallback,
    NULL
  };
  PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)glfwGetInstanceProcAddress(instance, "vkCreateDebugUtilsMessengerEXT");
  VkDebugUtilsMessengerEXT debugMessenger;
  pfnCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCreateInfo, &allocationCallbacks, &debugMessenger);

  pfnCreateDevice = (PFN_vkCreateDevice)glfwGetInstanceProcAddress(instance, "vkCreateDevice");
  if (!pfnCreateDevice) {
    printf("Failed to get vkCreateDevice\n");
    return 0;
  }
  else {
    printf("Found vkCreateDevice\n");
  }

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
  return instance;

}

VkRenderPass createRenderPass(VkDevice device, const VkAllocationCallbacks *allocator)
{
  uint32_t nAttachments = 1;
  const VkAttachmentDescription attachmentDescriptions[] = {{VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT,
							     VK_FORMAT_B8G8R8A8_UNORM,
							     VK_SAMPLE_COUNT_8_BIT,
							     VK_ATTACHMENT_LOAD_OP_CLEAR,
							     VK_ATTACHMENT_STORE_OP_STORE,
							     VK_ATTACHMENT_LOAD_OP_CLEAR,
							     VK_ATTACHMENT_STORE_OP_STORE,
                                                             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}};
  const VkAttachmentReference attachmentReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
  uint32_t nSubpasses = 1;

  const VkSubpassDescription subpasses[] = {{0,
                                             VK_PIPELINE_BIND_POINT_GRAPHICS,
					     0,
					     NULL,
					     1,
                                             &attachmentReference,
                                             NULL,
                                             NULL,
                                             0,
                                             NULL}};
  uint32_t nDependencies = 1;
  const VkSubpassDependency dependencies[] = {{
      VK_SUBPASS_EXTERNAL,
      0,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      0,
      VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      0
    }};
  const VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
							NULL,
							0,
							nAttachments,
							attachmentDescriptions,
							nSubpasses,
							subpasses,
							nDependencies,
							dependencies
  };
  VkRenderPass renderPass;
  if (VK_SUCCESS != pfnCreateRenderPass(device, &renderPassCreateInfo, allocator, &renderPass)) {
    printf("Render pass creation failed\n");
    return 0;
  }
  printf("Render pass created\n");

  return renderPass;
}

int readFile(const char * path, uint32_t * data, size_t * dataSizeInBytes);
int readFile(const char * path, uint32_t * data, size_t * dataSizeInBytes)
{
  FILE * file = fopen(path, "rb");
  if (!file) {
    printf("Unable to open file %s \n", path);
    return 0;
  }
  fseek(file, 0L, SEEK_END);
  size_t size = (size_t)ftell(file);
  int bufferTooSmall = *dataSizeInBytes <= size;
  *dataSizeInBytes = size;
  if (!data) {
    fclose(file);
    return 1;
  } else if (bufferTooSmall) {
    fclose(file);
    printf("Failed to open file %s : Buffer is too small\n", path);
    return 0;
  }
  rewind(file);
  fread(data, size, 1, file);
  fclose(file);

  return 1;
}

int main(void)
{
  if (!glfwInit()) {
    printf("GLFW initialisation failed\n");
    return -1;
  }
  if (!glfwVulkanSupported()) {
    printf("Vulkan not supported\n");
    return -1;
  }
  else printf("Vulkan supported\n");

  VkInstance instance = getFunctionPointers();

  uint32_t nPhysicalDevices = 32;
  VkPhysicalDevice physicalDevice[32];
  if (VK_SUCCESS != pfnEnumeratePhysicalDevices(instance, &nPhysicalDevices, physicalDevice))
    printf("Unsuccessful enumeratePhysicalDevices call");
  else printf("Num devices: %u\n", nPhysicalDevices);
  uint32_t queueFamilyCount = 16;
  VkQueueFamilyProperties queueFamilyProperties[16];
  pfnGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamilyCount, queueFamilyProperties);
  uint32_t presentationQueue = 0;
  uint32_t graphicsQueue = 0;
  int hasPresentationQueue = 0;
  int hasGraphicsQueue = 0;
  for (uint32_t i = 0; i < queueFamilyCount; ++i) {
    if (!glfwGetPhysicalDevicePresentationSupport(instance, *physicalDevice, i))
      printf("WOOT\n");
    else {
      printf("Supports Graphics\n");
      presentationQueue = i;
      hasPresentationQueue = 1;
    }
    if (queueFamilyProperties[i].queueCount > 0 && queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphicsQueue = i;
      hasGraphicsQueue = 1;
    }
    if (hasPresentationQueue && hasGraphicsQueue) break;
  }

  VkPhysicalDeviceFeatures physicalDeviceFeatures;
  pfnGetPhysicalDeviceFeatures(*physicalDevice, &physicalDeviceFeatures);
  uint32_t nDeviceQueueCreateInfos = 2;

  const float priority = 0.9f;
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


  const VkAllocationCallbacks allocationCallbacks = {NULL, allocator, reallocator, freeFunc, NULL, NULL};
  VkDevice device;
  if (VK_SUCCESS == pfnCreateDevice(*physicalDevice, &deviceCreateInfo, &allocationCallbacks, &device))
    printf("Logical device creation succeeded\n");

  VkQueue graphicsQueueHandle, presentationQueueHandle;
  pfnGetDeviceQueue(device, presentationQueue, 0, &presentationQueueHandle);
  pfnGetDeviceQueue(device, graphicsQueue, 0, &graphicsQueueHandle);

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  GLFWwindow * window = glfwCreateWindow(400, 400, "asdf", NULL, NULL);
  VkSurfaceKHR surface;
  if (VK_SUCCESS == glfwCreateWindowSurface(instance, window, NULL, &surface))
    printf("Window creation succeeded\n");

  const VkCommandPoolCreateInfo commandPoolCreateInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
							 NULL,
							 0,
                                                         graphicsQueue};
  VkCommandPool commandPool;
  if (VK_SUCCESS == pfnCreateCommandPool(device, &commandPoolCreateInfo, &allocationCallbacks, &commandPool))
      printf("Command pool creation succeeded\n");

  VkRenderPass renderPass = createRenderPass(device, &allocationCallbacks);

  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  pfnGetPhysicalDeviceSurfaceCapabilitiesKHR(*physicalDevice, surface, &surfaceCapabilities);
  VkSurfaceFormatKHR surfaceFormats[255];

  uint32_t nSurfaceFormats = sizeof(surfaceFormats);
  pfnGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice, surface, &nSurfaceFormats, surfaceFormats);
  if (nSurfaceFormats == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
    surfaceFormats[0].format = VK_FORMAT_B8G8R8A8_UNORM;
    surfaceFormats[0].colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  } else for (uint32_t i = 0; i < nSurfaceFormats; ++i) {
    if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      surfaceFormats[0].format = surfaceFormats[i].format;
    surfaceFormats[0].colorSpace = surfaceFormats[i].colorSpace;
  }

  const VkSwapchainCreateInfoKHR swapchainCreateInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
							NULL,
							0,
							surface,
							1,
							surfaceFormats->format,
							surfaceFormats->colorSpace,
							{400,400},
							1,
							VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
							VK_SHARING_MODE_EXCLUSIVE,
							0,
							NULL,
							surfaceCapabilities.currentTransform,
							VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
							VK_PRESENT_MODE_FIFO_KHR,
							VK_TRUE,
							VK_NULL_HANDLE
  };
  VkSwapchainKHR swapchain;
  if (VK_SUCCESS != pfnCreateSwapchainKHR(device, &swapchainCreateInfo, &allocationCallbacks, &swapchain))
    printf("Failed to create swap chain\n");
  else printf("Successfully created a swap chain \n");

  uint32_t nImages = 16;
  VkImage images[16];

  pfnGetSwapchainImagesKHR(device, swapchain, &nImages, images);
  if (nImages == 1)
    printf("nImages is 1\n");
  else printf("nImages is %u\n", nImages);

  VkImageView imageView[4];
  const VkComponentMapping componentMapping = {VK_COMPONENT_SWIZZLE_IDENTITY,
					       VK_COMPONENT_SWIZZLE_IDENTITY,
					       VK_COMPONENT_SWIZZLE_IDENTITY,
					       VK_COMPONENT_SWIZZLE_IDENTITY};

  const VkImageSubresourceRange imageSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT,
						         0,
						         1,
						         0,
						         1};

  VkImageViewCreateInfo imageViewCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
						     0,
						     0,
						     *images,
						     VK_IMAGE_VIEW_TYPE_2D,
				                     surfaceFormats[0].format,
						     componentMapping,
						     imageSubresourceRange};

  VkFramebuffer framebuffers[4];

  for (uint32_t i = 0; i < nImages; ++i) {
    imageViewCreateInfo.image = images[i];
    if (pfnCreateImageView && VK_SUCCESS != pfnCreateImageView(device, &imageViewCreateInfo, &allocationCallbacks, imageView + i))
      printf("Image view creation failed.\n");
    else printf("Image view creation succeeded\n");

    const VkFramebufferCreateInfo framebufferCreateInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                                         NULL,
                                                         0,
                                                         renderPass,
							 1,
							 imageView + i,
                                                         400,
                                                         400,
                                                         1};
    if (VK_SUCCESS == pfnCreateFramebuffer(device,
					   &framebufferCreateInfo,
					   &allocationCallbacks,
					   framebuffers + i))
      printf("Framebuffer creation successful\n");

  }

  VkCommandBuffer primaryCommandBuffers[4];
  VkCommandBufferAllocateInfo commandBufferAllocateInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
							   NULL,
							   commandPool,
							   VK_COMMAND_BUFFER_LEVEL_PRIMARY,
							   nImages};
  if (VK_SUCCESS == pfnAllocateCommandBuffers(device, &commandBufferAllocateInfo, primaryCommandBuffers))
    printf("Created primary command buffers\n");

  VkShaderModule vertexShaderModule, fragmentShaderModule;
  uint32_t codeData[2047];
  size_t codeDataSize = 2047;
  readFile("shader/vert.spv", codeData, &codeDataSize);

  VkShaderModuleCreateInfo shaderModuleCreateInfo = {
    VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    NULL,
    0,
    codeDataSize,
    codeData
  };
  if (VK_SUCCESS != pfnCreateShaderModule(device, &shaderModuleCreateInfo, &allocationCallbacks, &vertexShaderModule))
    printf("Failed to create vertex shader module\n");
  else printf("Successfully created vertex shader module\n");

  codeDataSize = 2047;
  readFile("shader/frag.spv", codeData, &codeDataSize);
  shaderModuleCreateInfo.codeSize = codeDataSize;
  if (VK_SUCCESS != pfnCreateShaderModule(device, &shaderModuleCreateInfo, &allocationCallbacks, &fragmentShaderModule))
    printf("Failed to create fragment shader module\n");
  else printf("Successfully created fragment shader module\n");
  uint32_t nPipelineStages = 2;
  VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[] = {
    {
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    NULL,
    0,
    VK_SHADER_STAGE_VERTEX_BIT,
    vertexShaderModule,
    "main",
    NULL
    },
    {
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    NULL,
    0,
    VK_SHADER_STAGE_FRAGMENT_BIT,
    fragmentShaderModule,
    "main",
    NULL
    }
  };
  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    NULL,
    0,
    0,
    NULL,
    0,
    NULL
  };
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    NULL,
    0,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    VK_FALSE
  };
  const VkViewport viewport =
    {
      0.0f,
      0.0f,
      400.0f,
      400.0f,
      0.0f,
      1.0f
    };

  const VkRect2D scissor = {
      {0,0},
      {400, 400}
  };
  const VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    NULL,
    0,
    1,
    &viewport,
    1,
    &scissor
  };

  const VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    NULL,
    0,
    VK_FALSE,
    VK_FALSE,
    VK_POLYGON_MODE_FILL,
    VK_CULL_MODE_BACK_BIT,
    VK_FRONT_FACE_CLOCKWISE,
    VK_FALSE,
    0.0f,
    0.0f,
    0.0f,
    1.0f
  };

  const VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    NULL,
    0,
    VK_SAMPLE_COUNT_1_BIT,
    VK_FALSE,
    1.0f,
    NULL,
    VK_FALSE,
    VK_FALSE
  };

  VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
    VK_FALSE,
      VK_BLEND_FACTOR_ONE,
      VK_BLEND_FACTOR_ZERO,
      VK_BLEND_OP_ADD,
      VK_BLEND_FACTOR_ONE,
      VK_BLEND_FACTOR_ZERO,
      VK_BLEND_OP_ADD,
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
  };
  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    NULL,
    0,
    VK_FALSE,
    VK_LOGIC_OP_COPY,
    1,
    &colorBlendAttachmentState,
    {0.0f, 0.0f, 0.0f, 0.0f}
  };

  VkDynamicState dynamicStates[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH
  };
  VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      NULL,
      0,
      sizeof(dynamicStates),
      dynamicStates
  };

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    NULL,
    0,
    0,
    NULL,
    0,
    NULL
  };
  VkPipelineLayout pipelineLayout;
  if (VK_SUCCESS != pfnCreatePipelineLayout(device, &pipelineLayoutCreateInfo, &allocationCallbacks, &pipelineLayout))
    printf("Pipeline layout creation failed\n");
  else printf("Pipeline layout creation succeeded\n");

  const VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
    VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    NULL,
    0,
    nPipelineStages,
    pipelineShaderStageCreateInfo,
    &vertexInputStateCreateInfo,
    &inputAssemblyStateCreateInfo,
    NULL,
    &viewportStateCreateInfo,
    &rasterizationStateCreateInfo,
    &multisampleStateCreateInfo,
    NULL,
    &colorBlendStateCreateInfo,
    &dynamicStateCreateInfo,
    pipelineLayout,
    renderPass,
    0,
    VK_NULL_HANDLE,
    0
  };

  VkPipeline pipeline;
  if (VK_SUCCESS != pfnCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, &allocationCallbacks, &pipeline))
    printf("Pipeline creation failed\n");
  else printf("Pipeline creation succeeded\n");

  VkSemaphore imageAvailable;
  VkSemaphore renderFinished;
  const VkSemaphoreCreateInfo semaphoreCreateInfo = {
    VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    NULL,
    0
  };
  if (VK_SUCCESS != pfnCreateSemaphore(device, &semaphoreCreateInfo, &allocationCallbacks, &imageAvailable))
    printf("Failed to create semaphore\n");
  else printf("Semaphore creation succeeded\n");
  if (VK_SUCCESS != pfnCreateSemaphore(device, &semaphoreCreateInfo, &allocationCallbacks, &renderFinished))
    printf("Failed to create semaphore\n");
  else printf("Semaphore creation succeeded\n");

  pfnResetCommandPool(device, commandPool, 0);
  VkClearColorValue clearColorValue = {{0.0f, 0.0f, 0.0f, 1.0f}};
  VkClearValue clearColor;
  clearColor.color = clearColorValue;
  VkRenderPassBeginInfo renderPassBeginInfo = {
    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    NULL,
    renderPass,
    NULL,
    {{0,0},{400,400}},
    1,
    &clearColor
  };
  for (uint32_t i = 0; i < nImages; ++i) {
    renderPassBeginInfo.framebuffer = framebuffers[i];
    pfnCmdBeginRenderPass(primaryCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    pfnCmdBindPipeline(primaryCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    pfnCmdDraw(primaryCommandBuffers[i], 3, 1, 0, 0);
    pfnCmdEndRenderPass(primaryCommandBuffers[i]);
    if (VK_SUCCESS != pfnEndCommandBuffer(primaryCommandBuffers[i]))
      printf("Failed to record command buffer\n");
    else printf("Command buffer recording succeeded\n");
  }

  while (!glfwWindowShouldClose(window)){
    uint32_t imageNext;
    glfwPollEvents();
    pfnAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &imageNext);
    const VkSemaphore waitSemaphores[] = {imageAvailable};
    const VkSemaphore signalSemaphores[] = {renderFinished};
    const VkPipelineStageFlags stageFlags[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo = {
      VK_STRUCTURE_TYPE_SUBMIT_INFO,
      NULL,
      1,
      waitSemaphores,
      stageFlags,
      1,
      primaryCommandBuffers + imageNext,
      1,
      signalSemaphores
    };

    VkResult result = pfnQueueSubmit(graphicsQueueHandle, 1, &submitInfo, VK_NULL_HANDLE);
    if (result != VK_SUCCESS)
      printf("Failed to submit draw\n");

    VkSwapchainKHR swapchains[] = {swapchain};
    VkPresentInfoKHR presentInfo = {
      VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      0,
      1,
      signalSemaphores,
      1,
      swapchains,
      &imageNext,
      &result
    };
    pfnQueuePresentKHR(presentationQueueHandle, &presentInfo);
    if (VK_SUCCESS != result)
      printf("Failed to present queue\n");
    pfnQueueWaitIdle(presentationQueueHandle);
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
