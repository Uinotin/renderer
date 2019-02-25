#include "node.h"

#include <string.h>

void NodeEventCallback(void * node, const void * eventData, uint32_t dataSize)
{
  NodeListener * nodeListener = (NodeListener *)node;
  memcpy(nodeListener->nodeIn, eventData, dataSize);
  *(nodeListener->isDirty) = 1;
}
