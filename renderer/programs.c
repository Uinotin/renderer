#include "programs.h"
#include "vulkanswapchain.h"
#include "rootnode.h"
#define PROGRAMNAMEBUFFERSIZE 1024
const char programNames[] = "root\0root_asset\0swapchain";
const Programs programs[] = {
  {&InitRootNode, &UpdateRootNode},
  {&InitRootNodeAssetReload, &RootNodeAssetReload},
  {&InitSwapchain, &CreateSwapchain}
};

void GetProgramsFromName(const char * name, Programs * returnPrograms)
{
  const char * names = programNames;
  for (size_t i = 0; i < sizeof(programs)/sizeof(Programs); ++i) {
    if (!strcmp(names, name)) {
      memcpy(returnPrograms, programs + i, sizeof(Programs));
      break;
    }
    names += strlen(names) + 1;
  }
}
