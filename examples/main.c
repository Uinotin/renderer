#include "graphics.h"
#include "nodetree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
  NodeTree rootTree;
  Window window;

  InitWindowEvents(&window);
  LoadNodeTree(&rootTree, "tree.txt", 0, window.events, window.numEvents);

  if(!CreateWindow(&window)) {
    DestroyWindow(&window);
    return -1;
  }

  while (!ShouldClose(&window)) {
    UpdateWindow(&window);
    UpdateNodeTree(&rootTree);
  }
  UpdateNodeTree(&rootTree);
  DestroyWindow(&window);
  return 0;
}
