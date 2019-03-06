#ifndef BMP_H
#define BMP_H

#include <stdint.h>
#include "readutils.h"

typedef struct Dib
{
  uint32_t headerSize;
  int32_t width, height;
  uint16_t colorPlanes;
  uint16_t bitsPerPixel;
  uint32_t compression;
  uint32_t imageSize;
  uint32_t resolutionX, resolutionY;
  uint32_t numColors;
  uint32_t numImportantColors;

  char colorSpaceType[4];
  char colorSpaceEndpointData[36];
  uint32_t redGamma, greenGamma, blueGamma;
  
  void * data;
} Dib;

int LoadBmp(Dib * dib, const char * filename);
void WriteBmp(Dib * dib, const char * filename);
void FreeDib(Dib * dib);

#endif ///BMP_H
