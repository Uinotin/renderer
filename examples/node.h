#ifndef NODE_H
#define NODE_H

#include "stdint.h"

struct Node;
typedef void (*UpdateProgram)(struct Node * node);
typedef uint32_t (*InitProgram)(struct Node * node,
				char ** memory,
				void ** childOutData,
				void ** childInitPrograms,
				void * out);
struct Node 
{
  uint32_t numChildren;
  void * locals; 
  void * out;
  UpdateProgram update;
};
typedef struct Node Node;

typedef struct NodeListener
{
  void * nodeIn;
  int * isDirty;
} NodeListener;

void NodeEventCallback(void * node, const void * eventData, uint32_t dataSize);

#endif ///NODE_H
