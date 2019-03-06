#include "bmp.h"
#include "dds.h"
#include "converter.h"
#include <stdio.h>

int main(void)
{
  Dib dib;
  Dds dds;
  LoadDds(&dds, "test.dds");
  GenerateDibFromDds(&dib, &dds);
  WriteBmp(&dib, "writetest.bmp");
  FreeDdsData(&dds);
  FreeDib(&dib);

  return 1;
}
