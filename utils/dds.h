#ifndef DDS_H
#define DDS_H

#include <stdint.h>

typedef struct PixelFormat
{
  uint32_t size;
  uint32_t flags;
  char id[4];
  uint32_t rgbBitCount;
  uint32_t rBitMask;
  uint32_t gBitMask;
  uint32_t bBitMask;
  uint32_t aBitMask;
} PixelFormat;

typedef struct Dds
{
  char id[4];
  uint32_t size;
  uint32_t flags;
  uint32_t height, width;
  uint32_t pitchOrLinearSize;
  uint32_t depth;
  uint32_t mipMapCount;
  PixelFormat pixelFormat;
  uint32_t caps;
  uint32_t caps2;
  void * data;
} Dds;

int LoadDds(Dds * dds, const char * filename);
int WriteDds(Dds * dds, const char * filename);
void FreeDdsData(Dds * dds);

#endif
