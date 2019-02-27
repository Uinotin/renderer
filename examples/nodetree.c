#include "nodetree.h"
#include "programs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Load trees.
void LoadNodeTree(NodeTree * nodeTree,
		  const char * filename,
		  size_t outDataSize,
		  Event ** events,
		  uint32_t numEvents)
{
  uint32_t childrenToBeUpdated[MAXNODETREEDEPTH] = {0};
  childrenToBeUpdated[0] = 1;
  void * ins[MAXNODETREEDEPTH][MAXNODECHILDREN];

  /// Tree is created root node first. As root node depends on all other nodes,
  /// branches will be updated leafs first.
  Node * currentNode = nodeTree->nodes + MAXNUMTREENODES;
  uint32_t numNodes = 0;
  char * memory = nodeTree->memory + NODETREEMEMORYSIZE - outDataSize;
  int * depths = nodeTree->depths + MAXNUMTREENODES;
  FILE * file = fopen(filename, "r");
  if (!file) {
    printf("Could not find file %s\n", filename);
    return;
  }

  /// This should later be changed to reading from binary
  /// For now, make sure that lines do not exceed 32 chars
  char line[32];
  size_t len = 32;
  char * linePtr = line;
  getline(&linePtr, &len, file);

  EventInit(&(nodeTree->readyEvent), linePtr, outDataSize);
  ins[0][0] = (void *)memory;

  int depth = 0;
  while (depth >= 0) {
    if (childrenToBeUpdated[depth]) {
      --childrenToBeUpdated[depth];
      --currentNode;
      ++numNodes;
      getline(&linePtr, &len, file);
      linePtr[strlen(linePtr) - 1] = '\0';
      size_t dataSize;
      Programs programs;
      size_t offsets[MAXNODECHILDREN];
      {
        const char eventString[] = "event_";
        if (strlen(linePtr) > strlen(eventString) &&
	    !strncmp(eventString, linePtr, strlen(eventString))) {
	  dataSize = 0;
	  programs.initProgram = NULL;
	  programs.update = NULL;
	  char * name = linePtr + strlen(eventString);
	  for (uint32_t i = 0; i < numEvents; ++i) {
	    if (!strcmp(events[i]->name, name)) {
	      Listener listener = {
		&NodeEventCallback,
		currentNode
	      };
	      EventAddListener(events[i], &listener);
	      break;
	    }
	  }
	} else {
	  programs.initProgram = NULL;
	  programs.update = NULL;
	  GetProgramsFromName(linePtr, &programs);
          dataSize = (*(programs.initProgram))(offsets);
	}
      }
      getline(&linePtr, &len, file);
      memory -= dataSize;
      uint32_t numChildren = (uint32_t)strtol(linePtr, NULL, 10);
      for (uint32_t i = 0; i < numChildren; ++i)
	ins[depth + 1][i] = (void *)(offsets[i] + memory);
      childrenToBeUpdated[depth + 1] = numChildren;
      currentNode->out = ins[depth][childrenToBeUpdated[depth]];
      currentNode->update = programs.update;
      currentNode->locals = (void *)(dataSize ? memory : NULL);
      *(--depths) = depth++;
    } else
      --depth;
  }

  nodeTree->firstLeafNodeDepth = depths;
  nodeTree->firstLeafNode = currentNode;
  nodeTree->numNodes = numNodes;

  fclose(file);
}

void UpdateNodeTree(NodeTree * nodeTree)
{
  Node * currentNode = nodeTree->firstLeafNode;
  int * depths = nodeTree->firstLeafNodeDepth;
  uint32_t numNodes = nodeTree->numNodes;
  int depth = -1;
  for (uint32_t i = 0; i < numNodes; ++i) {
    if (currentNode[i].isDirty && depth < depths[i])
      depth = depths[i];
    if (depth == depths[i]) {
      if (currentNode[i].update)
        (*(currentNode[i].update))(currentNode + i);
      --depth;
      currentNode[i].isDirty = 0;
    }
  }
}
