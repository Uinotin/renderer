#include "bmp.h"
#include "dds.h"
#include "converter.h"
#include <string.h>
#include <stdio.h>

int main(int argv, char * argc[])
{
  Dib dib;
  Dds dds;
  char * input = NULL;
  int isInputDds = 1;
  char * output = NULL;
  int isOutputDds = 1;

  {
    int strIsInput = 1;
    for (int i = 0; i < argv; ++i) {
      if (!strcmp(argc[i] + strlen(argc[i]) - 4, ".dds")) {
	if (strIsInput) {
	  input = argc[i];
	  isInputDds = 1;
	} else {
	  output = argc[i];
	  isOutputDds = 1;
	}
      } else if (!strcmp(argc[i] + strlen(argc[i]) - 4, ".bmp")) {
	if (strIsInput) {
	  input = argc[i];
	  isInputDds = 0;
	} else {
	  output = argc[i];
	  isOutputDds = 0;
	}
      }
      strIsInput = strcmp(argc[i], "-o");
    }
  }

  if (!input) {
    printf("Error: Valid input file name not specified\n");
    return 0;
  }
  if (!output) {
    printf("Error: Valid output file name not specified\n");
    return 0;
  }

  if (isInputDds && !isOutputDds) {
    if(!LoadDds(&dds, input))
      return 0;
    GenerateDibFromDds(&dib, &dds);
    if(!WriteBmp(&dib, output)) {
      FreeDdsData(&dds);
      FreeDib(&dib);
      return 0;
    }
  } else if (!isInputDds && isOutputDds) {
    if(!LoadBmp(&dib, input))
      return 0;
    GenerateDdsFromDib(&dds, &dib);
    if(!WriteDds(&dds, output)) {
      FreeDdsData(&dds);
      FreeDib(&dib);
      return 0;
    }
  } else {
    printf("Error: Input and output are the same type\n");
    return 0;
  }
  FreeDdsData(&dds);
  FreeDib(&dib);

  return 1;
}
