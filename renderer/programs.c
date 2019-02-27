#include "programs.h"
#include "vulkanswapchain.h"
#define PROGRAMNAMEBUFFERSIZE 1024
const char programNames[] = "swapchain";
const Programs programs[] = {
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
