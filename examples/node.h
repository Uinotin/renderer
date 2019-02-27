#ifndef NODE_H
#define NODE_H

#include "stdint.h"

struct Node;
typedef void (*UpdateProgram)(struct Node * node);
struct Node 
{
  void * locals; 
  void * out;
  int isDirty;
  UpdateProgram update;
};
typedef struct Node Node;

void NodeEventCallback(void * node, const void * eventData, uint32_t dataSize);

#endif ///NODE_H
