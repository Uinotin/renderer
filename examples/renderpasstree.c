#include "renderpasstree.h"

//Load trees. rootNodeName is not used as, as of still, we only have one root node.
void LoadNodeTree(NodeTree * nodeTree,
			InitProgram InitRootNode,
			uint32_t outDataSize,
			Event ** events,
			uint32_t numEvents)
{
  int32_t childrenToBeUpdated[MAXNODETREEDEPTH] = {0};
  InitProgram initPrograms[MAXNODETREEDEPTH][MAXNODECHILDREN];
  void * ins[MAXNODETREEDEPTH][MAXNODECHILDREN];

  /// Tree is created root node first. As root node depends on all other nodes,
  /// branches will be updated leafs first.
  Node * currentNode = nodeTree->nodes + MAXNUMTREENODES;
  char * memory = nodeTree->memory + NODETREEMEMORYSIZE - outDataSize;
  int * depths = nodeTree->depths;
  int * isDirty = nodeTree->isDirty;

  EventInit(&(nodeTree->readyEvent), "ready", outDataSize);
  ins[0][0] = (void *)memory;
  initPrograms[0][0] = InitRootNode;

  int depth = 0;
  while (depth >= 0) {
    if (initPrograms[depth][childrenToBeUpdated[depth]]) {
      --currentNode;
      childrenToBeUpdated[depth + 1] =
	(*(initPrograms[depth][childrenToBeUpdated[depth]]))
	            (currentNode,
		     &memory,
		     ins[depth][childrenToBeUpdated[depth]],
		     (void **)initPrograms[depth + 1],
		     ins[depth + 1]);
      currentNode->numChildren = childrenToBeUpdated[depth + 1];
      initPrograms[depth][childrenToBeUpdated[depth]] = NULL;
      *(depths++) = depth;
      *(isDirty++) = 1;
    }
    if (childrenToBeUpdated[depth + 1] >= 0)
      --childrenToBeUpdated[++depth];
    else
      --depth;
  }
}

void UpdateNodeTree(NodeTree * nodeTree)
{
  Node * currentNode = nodeTree->firstLeafNode;
  uint32_t numNodes = nodeTree->numNodes;
  int depth = 0;
  for (uint32_t i = 0; i < numNodes; ++i) {
    if (nodeTree->isDirty[i] && depth < nodeTree->depths[i])
      depth = nodeTree->depths[i];
    if (depth == nodeTree->depths[i]) {
      (*(currentNode[i].update))(currentNode + i);
      --depth;
    }
  }
}
