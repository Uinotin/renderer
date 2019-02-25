#ifndef ROOTNODE_H
#define ROOTNODE_H

#include "renderpass.h"

uint32_t initRootNode(RenderPass * renderPass,
		      char ** memory,
		      InitProgram * childInitPrograms,
		      void * out,
		      void ** childOutData,
		      UpdateProgram * updateProgram,
		      pthread_mutex_t treeMutex);

#endif ///ROOTNODE_H
