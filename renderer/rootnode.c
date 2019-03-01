#include <stdio.h>
#include <math.h>
#include "rootnode.h"
#include "vulkanswapchain.h"

typedef struct RootNodeAssetStart
{
  uint32_t numImageViews;
} RootNodeAssetStart;
typedef struct RootNodeAssetStartIn
{
  int var0;
} RootNodeAssetStartIn;

typedef struct RootNodeAssets
{
  VulkanContext var0;
  VulkanSwapchain var1;
  int var2;
  RootNodeAssetStart var3;
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
  float projMatrix[16];
  float location[3];
} VulkanRenderPass;

typedef struct RootNode
{
  VulkanContext var0;
  VulkanRenderPass var1;
  double var2;
  int var3;
} RootNode;

size_t InitRootNodeAssetReloadStart(size_t * childOutSizes)
{
  childOutSizes[0] = offsetof(RootNodeAssetStartIn, var0);
  return sizeof(RootNodeAssetStartIn);
}

void InitReloadValues(Node * node)
{
  RootNodeAssetStart * start = (RootNodeAssetStart *)node->out;
  RootNodeAssetStartIn * in = (RootNodeAssetStartIn *)node->locals;
  if (!in->var0)
    start->numImageViews = 0;
}

size_t InitRootNodeAssetReload(size_t * childOutSizes)
{
  childOutSizes[0] = offsetof(RootNodeAssets, var0);
  childOutSizes[1] = offsetof(RootNodeAssets, var1);
  childOutSizes[2] = offsetof(RootNodeAssets, var2);
  childOutSizes[3] = offsetof(RootNodeAssets, var3);
  return sizeof(RootNodeAssets);
}

size_t InitRootNode(size_t * childOutSizes)
{
  childOutSizes[0] = offsetof(RootNode, var0);
  childOutSizes[1] = offsetof(RootNode, var1);
  childOutSizes[2] = offsetof(RootNode, var2);
  childOutSizes[3] = offsetof(RootNode, var3);
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

static void Identity(float * mat)
{
  mat[0] = 1;
  mat[1] = 0;
  mat[2] = 0;
  mat[3] = 0;

  mat[4] = 0;
  mat[5] = 1;
  mat[6] = 0;
  mat[7] = 0;

  mat[8] = 0;
  mat[9] = 0;
  mat[10] = 1;
  mat[11] = 0;

  mat[12] = 0;
  mat[13] = 0;
  mat[14] = 0;
  mat[15] = 1;
}

static void Perspective(float * mat, float fov, float near, float far, float aspect)
{
  mat[0] = 1.0f/aspect;
}

void RootNodeAssetReload(Node * node)
{
  RootNodeAssets * in = (RootNodeAssets *)node->locals;
  VulkanRenderPass * out = (VulkanRenderPass *)node->out;
  /// todo: Cleanup
  if (in->var3.numImageViews) {
    pfnFreeCommandBuffers(in->var0.device,
			  in->var0.commandPool,
			  in->var3.numImageViews,
			  out->commandBuffers);
    pfnDestroyRenderPass(in->var0.device, out->handle, NULL);
    for (uint32_t i = 0; i < in->var3.numImageViews; ++i)
      pfnDestroyFramebuffer(in->var0.device, out->framebuffers[i], NULL);
    pfnDestroyPipelineLayout(in->var0.device, out->pipelineLayout, NULL);
    pfnDestroyPipeline(in->var0.device, out->pipeline, NULL);
  }
  if (in->var2)
    return;
  in->var3.numImageViews = in->var1.numImageViews;
  out->windowSize.width = in->var1.windowSize.width;
  out->windowSize.height = in->var1.windowSize.height;
  out->swapchainHandle = in->var1.handle;
  Identity(out->projMatrix);
  Perspective(out->projMatrix, 6.0, 0.1f, 10.0f,
	      out->windowSize.width/out->windowSize.height);
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
	in->var1.surfaceFormat.format,
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
      const VkFramebufferCreateInfo framebufferCreateInfo = {
        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        NULL,
        0,
        out->handle,
        1,
        in->var1.imageView + i,
        in->var1.windowSize.width,
        in->var1.windowSize.height,
        1
      };
      if (VK_SUCCESS == pfnCreateFramebuffer(in->var0.device,
					     &framebufferCreateInfo,
					     NULL,
					     out->framebuffers + i))
      printf("Framebuffer creation successful\n");
    }
  } /// Create framebuffers
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
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
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
      VK_CULL_MODE_NONE,
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
  
    VkPushConstantRange pushConstantRange = {
      VK_SHADER_STAGE_VERTEX_BIT,
      0,
      16*sizeof(float)
    };
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      NULL,
      0,
      0,
      NULL,
      1,
      &pushConstantRange
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

    pfnDestroyShaderModule(in->var0.device, fragmentShaderModule, NULL);
    pfnDestroyShaderModule(in->var0.device, vertexShaderModule, NULL);
  } /// End main pass pipeline creation
}

void UpdateRootNode(Node * node)
{
  RootNode * in = (RootNode *)node->locals;
  if (in->var3)
    return;
  uint32_t imageNext;
  pfnAcquireNextImageKHR(in->var0.device,
			 in->var1.swapchainHandle,
			 UINT64_MAX,
			 in->var0.imageAvailable,
			 VK_NULL_HANDLE,
			 &imageNext);

  {
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
    in->var1.handle,
    NULL,
    {{0,0},{in->var1.windowSize.width,in->var1.windowSize.height}},
    1,
    &clearColor
  };
    renderPassBeginInfo.framebuffer = in->var1.framebuffers[imageNext];

    pfnBeginCommandBuffer(in->var1.commandBuffers[imageNext], &commandBufferBeginInfo);
    pfnCmdPushConstants(in->var1.commandBuffers[imageNext],
			in->var1.pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			16*sizeof(float),
			in->var1.projMatrix);
    pfnCmdBeginRenderPass(in->var1.commandBuffers[imageNext], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    pfnCmdBindPipeline(in->var1.commandBuffers[imageNext], VK_PIPELINE_BIND_POINT_GRAPHICS, in->var1.pipeline);
    pfnCmdDraw(in->var1.commandBuffers[imageNext], 4, 1, 0, 0);
    pfnCmdEndRenderPass(in->var1.commandBuffers[imageNext]);
    if (VK_SUCCESS != pfnEndCommandBuffer(in->var1.commandBuffers[imageNext]))
      printf("Failed to record command buffer\n");
  }

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
