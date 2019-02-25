#include "graphics.h"
#include "renderpasstree.h"
#include "vulkanswapchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Event exitEvent, updateEvent, contextReadyEvent;

int main(void)
{
  NodeTree rootTree;
  EventInit(&exitEvent, "exit", sizeof(int));
  EventInit(&contextReadyEvent, "context", sizeof(VulkanContext));
  Event * events[] = { &exitEvent, &resizeEvent, &contextReadyEvent };

  VulkanContext context;
  if(!InitGraphics(&context)) {
    FreeGraphics(&context);
    return -1;
  }

  
  LoadNodeTree(&rootTree, &InitSwapchain, sizeof(VulkanSwapchain), events, 3);
  EventTrigger(&contextReadyEvent, &context);
  UpdateNodeTree(&rootTree);

  while (!glfwWindowShouldClose(context.window)) {
    glfwPollEvents();
    //EventTrigger(&update, getNextImageIndex(&context))
    //UpdateNodeTree(&rootTree);
  }

  //EventTrigger(&exit, 1);

  //UpdateNodeTree(&rootTree);

  FreeGraphics(&context);
  return 0;
}
