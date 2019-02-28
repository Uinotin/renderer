#ifndef NODETREE_H
#define NODETREE_H

#include "node.h"
#include "event.h"

#include <pthread.h>

/// As programmes don't run on magical fairy machines,
/// it's good to have a clear memory budget.
/// As of still, no need to allocate host memory dynamically
/// (other than what Vulkan allocates,
/// but we'll get to writing the alloc callbacks later).

#define NODETREEMEMORYSIZE 2024
#define MAXNUMTREENODES 64 
#define MAXNODETREEDEPTH 32
#define MAXNODECHILDREN 8

/// This is the core data structure for the programme.
/// All operations both loading and updating are done through this.
/// NodeTree contains Nodes that each contain
/// memory of a struct as void * locals, a partial or full memory of its
/// parent node void * out and a pointer to an update function UpdateFunction.
///
/// All nodes have an isDirty value and during NodeTree update
/// the invalidated nodes and their ancestors are updated.
///
/// All leaf nodes of this data structure listen to one or more events,
/// which invalidate them and modify their values.
///
/// During update, nodes take in values from void * locals data structure
/// and input them to the void * out data structure for their parent to process.
///
/// This allows for a system where only nodes, that have changed or depend on changed
/// nodes, will be updated and the rest contain their previous states
/// as long as those are valid.

typedef struct NodeTree
{
  Node * firstLeafNode;
  int * firstLeafNodeDepth;
  int depths[MAXNUMTREENODES];

  Node nodes[MAXNUMTREENODES];
  uint32_t numNodes;

  char memory[NODETREEMEMORYSIZE];

  pthread_mutex_t mutex;
  Event readyEvent;
} NodeTree;

void LoadNodeTree(NodeTree * nodeTree,
		  const char * filename,
		  size_t outDataSize,
		  Event * events,
		  uint32_t numEvents);

void UpdateNodeTree(NodeTree * nodeTree);

#endif /// NODE_H
