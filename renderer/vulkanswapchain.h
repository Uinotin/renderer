#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#include <stdint.h>
#include "window.h"
#include "node.h"

typedef struct VulkanSwapchain
{
  VkSwapchainKHR handle;
  VkSurfaceFormatKHR surfaceFormat;
  VkImageView imageView[8];
  VkImageView depthImageView;
  uint32_t numImageViews;
  WindowSize windowSize;
} VulkanSwapchain;

size_t InitSwapchain(size_t * childOutData);
void CreateSwapchain(Node * node);
void StartSwapchainLocals(Node * node);
size_t InitSwapchainLocals(size_t * childOutData);

#endif /// VULKANSWAPCHAIN_H
