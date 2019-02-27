#include "graphics.h"
#include "nodetree.h"
#include "vulkanswapchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Event exitEvent, contextReadyEvent;

int main(void)
{
  NodeTree rootTree;
  EventInit(&exitEvent, "exit", sizeof(int));

  VulkanContext context;
  int closing = 0;
  {
    /// todo: Event going out of scope leaves an extra ListenerNode,
    /// that does nothing, into memory.
    /// Find a better solution for this.
    Event contextReadyEvent;
    EventInit(&contextReadyEvent, "context", sizeof(VulkanContext));
    EventInit(&resizeEvent, "resize", sizeof(WindowSize));

    Event * events[] = { &exitEvent, &resizeEvent, &contextReadyEvent };
    LoadNodeTree(&rootTree, "tree.txt", 0, events, 3);

    if(!InitGraphics(&context)) {
      FreeGraphics(&context);
      return -1;
    }
    EventTrigger(&contextReadyEvent, &context);
    EventTrigger(&exitEvent, &closing);
  }

  while (!glfwWindowShouldClose(context.window)) {
    glfwPollEvents();
    UpdateNodeTree(&rootTree);
  }

  closing = 1;
  EventTrigger(&exitEvent, &closing);
  UpdateNodeTree(&rootTree);
  FreeGraphics(&context);
  return 0;
}
