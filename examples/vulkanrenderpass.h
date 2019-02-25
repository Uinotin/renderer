#ifndef VULKANRENDERPASS_H
#define VULKANRENDERPASS_H

uint32_t InitVulkanRenderPass(Node * node,
		           char ** memory,
		           InitProgram * childInitPrograms,
		           void * out,
		           void ** childOutData,
		           UpdateProgram * updateProgram);


typedef struct VulkanRenderPass 
{
  VkRenderPass handle;
  VkPipeline pipeline;
  VkCommandBuffer commandBuffer;
  VkFramebuffer framebuffers[8];
  
  int hasBeenUpdated;
} VulkanRenderPass;

#endif
