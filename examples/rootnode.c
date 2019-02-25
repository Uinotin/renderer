#include "rootnode.h"
#include "vulkanrenderpass.h"

typedef struct RootNodeReloadNode
{
  VulkanSwapchainAndContext vulkanContext;
  VulkanRenderPass renderPass;
} RootNodeReloadNode;

typedef struct RootNode
{
  RootNodeAssets assets;
  
  VulkanSwapchainAndContext vulkanContext;
} RootNode;

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

uint32_t InitAssetReloadNode(Node * node,
			     char ** memory,
			     InitProgram * childInitPrograms,
			     void * out,
			     void childOutData,
			     UpdateProgram * updateProgram)
{

}

void ReloadAssets(Node * node)
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
    renderPass,
    NULL,
    {{0,0},{400,400}},
    1,
    &clearColor
  };
  for (uint32_t i = 0; i < nImages; ++i) {
    renderPassBeginInfo.framebuffer = framebuffers[i];

    pfnBeginCommandBuffer(primaryCommandBuffers[i], &commandBufferBeginInfo);
    pfnCmdBeginRenderPass(primaryCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    pfnCmdBindPipeline(primaryCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    pfnCmdDraw(primaryCommandBuffers[i], 3, 1, 0, 0);
    pfnCmdEndRenderPass(primaryCommandBuffers[i]);
    if (VK_SUCCESS != pfnEndCommandBuffer(primaryCommandBuffers[i]))
      printf("Failed to record command buffer\n");
    else printf("Command buffer recording succeeded\n");
  }
}

void updateRootNode(RenderPass * renderPass)
{

  
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
