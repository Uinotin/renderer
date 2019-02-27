#include "node.h"

#include <string.h>

void NodeEventCallback(void * node, const void * eventData, uint32_t dataSize)
{
  Node * listenerNode = (Node *)node;
  memcpy(listenerNode->out, eventData, dataSize);
  listenerNode->isDirty = 1;
}
