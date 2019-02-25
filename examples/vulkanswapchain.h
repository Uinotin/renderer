#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H
#include "graphics.h"

#include <stdint.h>
#include "node.h"

typedef struct VulkanSwapchain
{
  VkSwapchainKHR handle;
  VkImageView imageView[8];
  uint32_t numImageViews;
} VulkanSwapchain;

uint32_t InitSwapchain(Node * node,
		       char ** memory,
		       void ** childOutData,
		       void ** updateProgram,
		       void * out);

#endif /// VULKANSWAPCHAIN_H
