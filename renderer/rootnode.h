#ifndef ROOTNODE_H
#define ROOTNODE_H

#include "window.h"
#include "node.h"

size_t InitRootNodeAssetReload(size_t * childOutSizes);
size_t InitRootNode(size_t * childOutSizes);
void RootNodeAssetReload(Node * node);
void UpdateRootNode(Node * node);
size_t InitRootNodeAssetReloadStart(size_t * childOutSizes);
void InitReloadValues(Node * node);

#endif ///ROOTNODE_H
