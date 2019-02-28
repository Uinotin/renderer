#include <stdio.h>
#include "rootnode.h"
#include "vulkanswapchain.h"

typedef struct RootNodeAssets
{
  VulkanContext var0;
  VulkanSwapchain var1;
} RootNodeAssets;

typedef struct VulkanRenderPass 
{
  VkRenderPass handle;
  VkPipeline pipeline;
  VkPipelineLayout pipelineLayout;
  VkCommandBuffer commandBuffers[8];
  VkFramebuffer framebuffers[8];
  VkSwapchainKHR swapchainHandle;
  WindowSize windowSize;
} VulkanRenderPass;

typedef struct RootNode
{
  VulkanContext var0;
  VulkanRenderPass var1;
  double var2;
} RootNode;

size_t InitRootNodeAssetReload(size_t * childOutSizes)
{
  childOutSizes[0] = offsetof(RootNodeAssets, var0);
  childOutSizes[1] = offsetof(RootNodeAssets, var1);
  return sizeof(RootNodeAssets);
}

size_t InitRootNode(size_t * childOutSizes)
{
  childOutSizes[0] = offsetof(RootNode, var0);
  childOutSizes[1] = offsetof(RootNode, var1);
  childOutSizes[2] = offsetof(RootNode, var2);
  return sizeof(RootNode);
}

static int ReadFile(const char * path, uint32_t * data, size_t * dataSizeInBytes)
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

static VkShaderModule LoadShaderModule(const VkDevice device, const char * filename)
{
  VkShaderModule shaderModule;
  uint32_t codeData[MAXSHADERFILESIZE];
  size_t codeDataSize = MAXSHADERFILESIZE;
  ReadFile(filename, codeData, &codeDataSize);

  VkShaderModuleCreateInfo shaderModuleCreateInfo = {
    VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    NULL,
    0,
    codeDataSize,
    codeData
  };
  if (VK_SUCCESS !=
      pfnCreateShaderModule(device, &shaderModuleCreateInfo, NULL, &shaderModule)
      )
    printf("Failed to create shader module\n");
  else printf("Successfully created shader module\n");

  return shaderModule;
}

void RootNodeAssetReload(Node * node)
{
  RootNodeAssets * in = (RootNodeAssets *)node->locals;
  VulkanRenderPass * out = (VulkanRenderPass *)node->out;
  out->swapchainHandle = in->var1.handle;
  { ///Allocate command buffers
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      NULL,
      in->var0.commandPool,
      VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      in->var1.numImageViews
    };

    if (VK_SUCCESS ==
        pfnAllocateCommandBuffers(in->var0.device,
				  &commandBufferAllocateInfo,
				  out->commandBuffers))
    printf("Created primary command buffers\n");
  } ///End command buffer allocation
  { ///Create a render pass
    uint32_t nAttachments = 1;
    const VkAttachmentDescription attachmentDescriptions[] = {
      {
	VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT,
	VK_FORMAT_B8G8R8A8_UNORM,
	VK_SAMPLE_COUNT_1_BIT,
	VK_ATTACHMENT_LOAD_OP_CLEAR,
	VK_ATTACHMENT_STORE_OP_STORE,
	VK_ATTACHMENT_LOAD_OP_CLEAR,
	VK_ATTACHMENT_STORE_OP_STORE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      }
    };

    const VkAttachmentReference attachmentReference = {
      0,
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    uint32_t nSubpasses = 1;

    const VkSubpassDescription subpasses[] = {
      {
       0,
       VK_PIPELINE_BIND_POINT_GRAPHICS,
       0,
       NULL,
       1,
       &attachmentReference,
       NULL,
       NULL,
       0,
       NULL
      }
    };
    uint32_t nDependencies = 1;
    const VkSubpassDependency dependencies[] = {
      {
        VK_SUBPASS_EXTERNAL,
        0,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0,
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        0
      }
    };
    const VkRenderPassCreateInfo renderPassCreateInfo = {
      VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      NULL,
      0,
      nAttachments,
      attachmentDescriptions,
      nSubpasses,
      subpasses,
      nDependencies,
      dependencies
    };
    if (VK_SUCCESS != pfnCreateRenderPass(in->var0.device, &renderPassCreateInfo, NULL, &(out->handle))) {
      printf("Render pass creation failed\n");
      return;
    }
    printf("Render pass created\n");
  } /// End render pass creation

  { /// Create framebuffers
  for (uint32_t i = 0; i < in->var1.numImageViews; ++i) {
    const VkFramebufferCreateInfo framebufferCreateInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                                         NULL,
                                                         0,
                                                         out->handle,
							 1,
							 in->var1.imageView + i,
                                                         in->var1.windowSize.width,
                                                         in->var1.windowSize.height,
                                                         1};
    if (VK_SUCCESS == pfnCreateFramebuffer(in->var0.device,
					   &framebufferCreateInfo,
					   NULL,
					   out->framebuffers + i))
      printf("Framebuffer creation successful\n");

  } /// Create framebuffers
  }
  { /// Create pipeline for main pass
    VkShaderModule vertexShaderModule = LoadShaderModule(in->var0.device, "shader/vert.spv");
    VkShaderModule fragmentShaderModule = LoadShaderModule(in->var0.device, "shader/frag.spv");

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
        (float)in->var1.windowSize.width,
        (float)in->var1.windowSize.height,
        0.0f,
        1.0f
      };
  
    const VkRect2D scissor = {
        {0,0},
        {in->var1.windowSize.width, in->var1.windowSize.height}
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
      VK_COLOR_COMPONENT_R_BIT |
      VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT |
      VK_COLOR_COMPONENT_A_BIT
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
      VK_DYNAMIC_STATE_LINE_WIDTH
    };
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        NULL,
        0,
        1,
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
    if (VK_SUCCESS !=
	pfnCreatePipelineLayout(in->var0.device,
				&pipelineLayoutCreateInfo,
				NULL,
				&(out->pipelineLayout)))
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
      out->pipelineLayout,
      out->handle,
      0,
      VK_NULL_HANDLE,
      0
    };

    if (VK_SUCCESS != pfnCreateGraphicsPipelines(in->var0.device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, NULL, &out->pipeline))
      printf("Pipeline creation failed\n");
    else printf("Pipeline creation succeeded\n");
  } /// End main pass pipeline creation

  VkClearColorValue clearColorValue = {{1.0f, 0.0f, 0.0f, 1.0f}};
  VkClearValue clearColor;
  clearColor.color = clearColorValue;
  VkCommandBufferBeginInfo commandBufferBeginInfo = {
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    0,
    VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    NULL
  };
  VkRenderPassBeginInfo renderPassBeginInfo = {
    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    NULL,
    out->handle,
    NULL,
    {{0,0},{in->var1.windowSize.width,in->var1.windowSize.height}},
    1,
    &clearColor
  };
  for (uint32_t i = 0; i < in->var1.numImageViews; ++i) {
    renderPassBeginInfo.framebuffer = out->framebuffers[i];

    pfnBeginCommandBuffer(out->commandBuffers[i], &commandBufferBeginInfo);
    pfnCmdBeginRenderPass(out->commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    pfnCmdBindPipeline(out->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, out->pipeline);
    pfnCmdDraw(out->commandBuffers[i], 3, 1, 0, 0);
    pfnCmdEndRenderPass(out->commandBuffers[i]);
    if (VK_SUCCESS != pfnEndCommandBuffer(out->commandBuffers[i]))
      printf("Failed to record command buffer\n");
    else printf("Command buffer recording succeeded\n");
  }
}

void UpdateRootNode(Node * node)
{
  RootNode * in = (RootNode *)node->locals;
  uint32_t imageNext;
  pfnAcquireNextImageKHR(in->var0.device,
			 in->var1.swapchainHandle,
			 UINT64_MAX,
			 in->var0.imageAvailable,
			 VK_NULL_HANDLE,
			 &imageNext);

  const VkSemaphore waitSemaphores[] = {in->var0.imageAvailable};
  const VkSemaphore signalSemaphores[] = {in->var0.renderFinished};
  const VkPipelineStageFlags stageFlags[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSubmitInfo submitInfo = {
    VK_STRUCTURE_TYPE_SUBMIT_INFO,
    NULL,
    1,
    waitSemaphores,
    stageFlags,
    1,
    in->var1.commandBuffers + imageNext,
    1,
    signalSemaphores
  };

  VkResult result = pfnQueueSubmit(in->var0.graphicsQueue,
				   1, &submitInfo, VK_NULL_HANDLE);
  if (result != VK_SUCCESS)
    printf("Failed to submit draw\n");

  VkSwapchainKHR swapchains[] = {in->var1.swapchainHandle};
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
  pfnQueuePresentKHR(in->var0.presentationQueue, &presentInfo);
  if (VK_SUCCESS != result)
    printf("Failed to present queue\n");
  pfnQueueWaitIdle(in->var0.presentationQueue);
}
/*
uint32_t InitRootNode(Node * node,
		      char ** memory,
		      InitProgram * childInitPrograms,
		      void * out,
		      void ** childOutData,
		      UpdateProgram * updateProgram)
{
  *memory -= sizeof(RootNode);
  node->locals = (RootNode *)*memory;
  RootNode * rootNode = (RootNode *)node->locals;
  node->out = NULL;
  node->update = &updateRootNode;
  childOutData[0] = (void *)&(rootNode->passInfo);
  childOutData[1] = (void *)&(rootNode->vulkanContextInfo);
  childInitPrograms[0] = (void *)&InitVulkanRenderPass;

  return 2;
}
uint32_t InitRootNode(RenderPass * renderPass,
		      char ** memory,
		      char ** sharedMemory,
		      InitProgram * childInitPrograms,
		      char ** childOutData,
		      UpdateProgram * updateProgram,
		      UpdateProgram * freeProgram)
{
  *freeProgram = &FreeRootNode;
  *updateProgram = &UpdateRootNode;

  *memory -= sizeof(RootNode);
  RootNode * rootNode = (RootNode *)(*memory);
  renderPass->locals = (void*)rootNode;
  renderPass->out = NULL;
  renderPass->numChildren = 0;

  return 0;
}

void UpdateRootNode(RenderPass * renderPass)
{
  RootNode * rootNode = (RootNode *)renderPass->locals;
  uint32_t imageNext;
  pfnAcquireNextImageKHR(rootNode->vulkanContext.device,
			 rootNode->swapchain.handle,
			 UINT64_MAX,
			 rootNode->vulkanContext.imageAvailable,
			 VK_NULL_HANDLE,
			 &imageNext);

  const VkSemaphore waitSemaphores[] = {rootNode->vulkanContext.imageAvailable};
  const VkSemaphore signalSemaphores[] = {rootNode->vulkanContext.renderFinished};
  const VkPipelineStageFlags stageFlags[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSubmitInfo submitInfo = {
    VK_STRUCTURE_TYPE_SUBMIT_INFO,
    NULL,
    1,
    waitSemaphores,
    stageFlags,
    1,
    rootNode->passInfo.commandBuffer,
    1,
    signalSemaphores
  };

  VkResult result = pfnQueueSubmit(rootNode->vulkanContext.graphicsQueueHandle,
				   1, &submitInfo, VK_NULL_HANDLE);
  if (result != VK_SUCCESS)
    printf("Failed to submit draw\n");

  VkSwapchainKHR swapchains[] = {rootNode->vulkanContext.swapchain};
  VkPresentInfoKHR presentInfo = {
      VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      0,
      1,
      rootNode->vulkanContext.signalSemaphores,
      1,
      swapchains,
      &imageNext,
      &result
  };
  pfnQueuePresentKHR(rootNode->vulkanContext.presentationQueueHandle, &presentInfo);
  if (VK_SUCCESS != result)
    printf("Failed to present queue\n");
  pfnQueueWaitIdle(rootNode->vulkanContext.presentationQueueHandle);
}

void FreeRootNode(RenderPass * renderPass)
{
}

*/
