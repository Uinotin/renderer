#include "vulkanrenderpass.h"

uint32_t InitVulkanRenderPass(Node * node,
		           char ** memory,
		           InitProgram * childInitPrograms,
		           void * out,
		           void ** childOutData,
		           UpdateProgram * updateProgram)
{
}

void Reload
static int readFile(const char * path, uint32_t * data, size_t * dataSizeInBytes)
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

VkShaderModule loadShaderModule(const char * filename)
{
  VkShaderModule shaderModule;
  uint32_t codeData[MAXSHADERFILESIZE];
  size_t codeDataSize = MAXSHADERFILESIZE;
  readFile(filename, codeData, &codeDataSize);

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
    printf("Failed to create vertex shader module\n");
  else printf("Successfully created vertex shader module\n");

  return shaderModule;
}

void LoadPass(void)
{
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
    if (VK_SUCCESS != pfnCreateRenderPass(device, &renderPassCreateInfo, NULL, &renderPass)) {
      printf("Render pass creation failed\n");
      return 0;
    }
    printf("Render pass created\n");
  } /// End render pass creation

  { /// Create pipeline for main pass
    VkShaderModule vertexShaderModule = loadShaderModule("shader/vert.spv");
    VkShaderModule fragmentShaderModule = loadShaderModule("shader/frag.spv");

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
    VkPipelineLayout pipelineLayout;
    if (VK_SUCCESS != pfnCreatePipelineLayout(device, &pipelineLayoutCreateInfo, NULL, &pipelineLayout))
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

    if (VK_SUCCESS != pfnCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, NULL, &pipeline))
      printf("Pipeline creation failed\n");
    else printf("Pipeline creation succeeded\n");
  } /// End main pass pipeline creation
}

void createCommandBuffers()
{
  VkCommandBuffer primaryCommandBuffers[4];
  VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    NULL,
    commandPool,
    VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    nImages
  };



  if (VK_SUCCESS ==
      pfnAllocateCommandBuffers(device,
				&commandBufferAllocateInfo,
				primaryCommandBuffers))
    printf("Created primary command buffers\n");
}

void CreateSwapchainFramebuffers(VulkanRenderPass * renderPass)
{
  for (uint32_t i = 0; i < numSwapchainImages; ++i) {
    imageViewCreateInfo.image = images[i];
    if (pfnCreateImageView && VK_SUCCESS != pfnCreateImageView(device, &imageViewCreateInfo, NULL, imageView + i))
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
					   NULL,
					   renderPass->framebuffers + i))
      printf("Framebuffer creation successful\n");

  }
}
