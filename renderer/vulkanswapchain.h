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

size_t InitSwapchain(size_t * childOutData);
void CreateSwapchain(Node * node);

#endif /// VULKANSWAPCHAIN_H
