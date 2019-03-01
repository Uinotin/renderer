#include "vulkanswapchain.h"

#include <stdio.h>

typedef struct SwapchainCreateIn
{
  int var0;
} SwapchainCreateIn;
typedef struct SwapchainCreateLocals
{
  uint32_t numImageViews;
} SwapchainCreateLocals;

typedef struct SwapchainIn
{
  VulkanContext var0;
  WindowSize var1;
  int var2;
  SwapchainCreateLocals var3;
} SwapchainIn;


void StartSwapchainLocals(Node * node)
{
  SwapchainCreateLocals * out = (SwapchainCreateLocals *)node->out;
  SwapchainCreateIn * in = (SwapchainCreateIn *)node->locals;
  if (!in->var0)
    out->numImageViews = 0;
}

void CreateSwapchain(Node * node)
{
  SwapchainIn * in = (SwapchainIn *)node->locals;
  VulkanSwapchain * out = (VulkanSwapchain *)node->out;
  if (in->var3.numImageViews) {
    for (uint32_t i = 0; i < in->var3.numImageViews; ++i)
      if (out->imageView[i] != VK_NULL_HANDLE)
        pfnDestroyImageView(in->var0.device, out->imageView[i], NULL);
    if (out->handle != VK_NULL_HANDLE)
      pfnDestroySwapchainKHR(in->var0.device, out->handle, NULL);
    in->var3.numImageViews = 0;
  }
  if (in->var2)
    return;

  out->windowSize.width = in->var1.width;
  out->windowSize.height = in->var1.height;
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  pfnGetPhysicalDeviceSurfaceCapabilitiesKHR(in->var0.physicalDevice,
					     in->var0.surface,
					     &surfaceCapabilities);
  out->surfaceFormat.format = VK_FORMAT_UNDEFINED;
  VkSurfaceFormatKHR surfaceFormats[255];
  uint32_t nSurfaceFormats = sizeof(surfaceFormats)/sizeof(surfaceFormats[0]);
  pfnGetPhysicalDeviceSurfaceFormatsKHR(in->var0.physicalDevice,
					in->var0.surface,
					&nSurfaceFormats,
					surfaceFormats);
  if (nSurfaceFormats == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
    out->surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    out->surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  } else for (uint32_t i = 0; i < nSurfaceFormats; ++i) {
    if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM &&
	surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      out->surfaceFormat.format = surfaceFormats[i].format;
    out->surfaceFormat.colorSpace = surfaceFormats[i].colorSpace;
  }
  if (out->surfaceFormat.format == VK_FORMAT_UNDEFINED) {
    printf("Could not find surface format\n");
  }

  const VkSwapchainCreateInfoKHR swapchainCreateInfo = {
    VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    NULL,
    0,
    in->var0.surface,
    2,
    out->surfaceFormat.format,
    out->surfaceFormat.colorSpace,
    {in->var1.width, in->var1.height},
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

  if (VK_SUCCESS !=
      pfnCreateSwapchainKHR(in->var0.device,
			    &swapchainCreateInfo,
			    NULL,
			    &(out->handle)))
    printf("Failed to create swap chain\n");
  else printf("Successfully created a swap chain \n");

  out->numImageViews = 8;
  VkImage images[8];

  pfnGetSwapchainImagesKHR(in->var0.device,
			   out->handle,
			   &(out->numImageViews),
			   images);
  in->var3.numImageViews = out->numImageViews;
  printf("Swapchain has %u images\n", out->numImageViews);

  const VkComponentMapping componentMapping = {VK_COMPONENT_SWIZZLE_IDENTITY,
					       VK_COMPONENT_SWIZZLE_IDENTITY,
					       VK_COMPONENT_SWIZZLE_IDENTITY,
					       VK_COMPONENT_SWIZZLE_IDENTITY};

  const VkImageSubresourceRange imageSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT,
						         0,
						         1,
						         0,
						         1};

  VkImageViewCreateInfo imageViewCreateInfo = {
    VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    0,
    0,
    *images,
    VK_IMAGE_VIEW_TYPE_2D,
    out->surfaceFormat.format,
    componentMapping,
    imageSubresourceRange
  };

  for (uint32_t i = 0; i < out->numImageViews; ++i) {
    imageViewCreateInfo.image = images[i];
    if (pfnCreateImageView && VK_SUCCESS != pfnCreateImageView(in->var0.device, &imageViewCreateInfo, NULL, out->imageView + i))
      printf("Image view creation failed.\n");
    else printf("Image view creation succeeded\n");
  }
}

size_t InitSwapchain(size_t * childOutData)
{
  /// todo: Make a macro for this
  childOutData[0] = offsetof(SwapchainIn, var0);
  childOutData[1] = offsetof(SwapchainIn, var1);
  childOutData[2] = offsetof(SwapchainIn, var2);
  childOutData[3] = offsetof(SwapchainIn, var3);
  return sizeof(SwapchainIn);
}

size_t InitSwapchainLocals(size_t * childOutData)
{
  return sizeof(SwapchainCreateLocals);
}
