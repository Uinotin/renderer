#include <stdio.h>
#include <stdlib.h>
#include "dds.h"
#include "readutils.h"
static void PrintDds(Dds * dds)
{
  printf("%c%c%c%c\n", dds->id[0], dds->id[1], dds->id[2], dds->id[3]);
  printf("Dds header size: %u\n", dds->size);
  printf("Dds flags: %x\n", dds->flags);
  printf("Dds height: %u\n", dds->height);
  printf("Dds width: %u\n", dds->width);
  printf("Dds image size: %u\n", dds->pitchOrLinearSize);
  printf("Dds depth: %u\n", dds->depth);
  printf("Dds mip map count: %u\n", dds->mipMapCount);
  printf("Dds pixel format header size: %u\n", dds->pixelFormat.size);
  printf("Dds pixel format flags: %x\n", dds->pixelFormat.flags);
  printf("Dds pixel format fourcc: %c%c%c%c\n", dds->pixelFormat.id[0],
	                                        dds->pixelFormat.id[1],
	                                        dds->pixelFormat.id[2],
	                                        dds->pixelFormat.id[3]);

  printf("caps: %u\n", dds->caps);
  printf("caps2: %u\n", dds->caps2);
}

static int SanityCheck(Dds * dds)
{
  if(dds->size != 124) {
    printf("Error: Dds file header size is not 124\n");
    return 0;
  }
  if(dds->height > 4096 || dds->width > 4096) {
    printf("Error: Image dimensions are too large\n");
    return 0;
  }
  if(((dds->height % 4) + (dds->width % 4)) || dds->width < 4 || dds->height < 4) {
    printf("Error: Dimensions are not divisable by 4\n");
    return 0;
  }
  if(dds->id[0] != 'D' ||
     dds->id[1] != 'D' ||
     dds->id[2] != 'S' ||
     dds->id[3] != ' ') {
    printf("Error: File type is not DDS\n");
    return 0;
  }
  if(dds->pixelFormat.id[0] != 'D' ||
     dds->pixelFormat.id[1] != 'X' ||
     dds->pixelFormat.id[2] != 'T' ||
     dds->pixelFormat.id[3] != '1') {
    printf("Error: Pixel format is not DXT1\n");
    return 0;
  }
  if(dds->pitchOrLinearSize > MAX_IMAGE_SIZE/2) {
    return 0;
  }
  return 1;
}

int LoadDds(Dds * dds, const char * filename)
{
  FILE * file = fopen(filename, "rb");
  if (!file) {
    printf("Error: Could not open file %s\n", filename);
    return 0;
  }

  fread(dds->id, 1, 4, file);
  readInt(file, &(dds->size), 4);
  readInt(file, &(dds->flags), 4);
  readInt(file, &(dds->height), 4);
  readInt(file, &(dds->width), 4);
  readInt(file, &(dds->pitchOrLinearSize), 4);
  readInt(file, &(dds->depth), 4);
  readInt(file, &(dds->mipMapCount), 4);

  fseek(file, 11*4, SEEK_CUR);

  readInt(file, &(dds->pixelFormat.size), 4);
  readInt(file, &(dds->pixelFormat.flags), 4);
  fread(dds->pixelFormat.id, 1, 4, file);
  readInt(file, &(dds->pixelFormat.rgbBitCount), 4);
  readInt(file, &(dds->pixelFormat.rBitMask), 4);
  readInt(file, &(dds->pixelFormat.gBitMask), 4);
  readInt(file, &(dds->pixelFormat.bBitMask), 4);
  readInt(file, &(dds->pixelFormat.aBitMask), 4);

  readInt(file, &(dds->caps), 4);
  readInt(file, &(dds->caps2), 4);
  fseek(file, 3*4, SEEK_CUR);

  PrintDds(dds);
  if (!SanityCheck(dds)) {
    fclose(file);
    return 0;
  }
  dds->data = malloc(dds->pitchOrLinearSize);
  fread(dds->data, dds->pitchOrLinearSize, 1, file);
  fclose(file);

  return 1;
}
int WriteDds(Dds * dds, const char * filename)
{
  uint8_t zero[11*4] = {0};
  FILE * file = fopen(filename, "wb");
  if (!file) {
    printf("Error: Could not open file %s\n", filename);
    return 0;
  }

  PrintDds(dds);
  if (!SanityCheck(dds))
    return 0;
  char id[4] = { 'D', 'D', 'S', ' '};
  fwrite(id, 1, 4, file);
  writeInt(file, &(dds->size), 4);
  writeInt(file, &(dds->flags), 4);
  writeInt(file, &(dds->height), 4);
  writeInt(file, &(dds->width), 4);
  writeInt(file, &(dds->pitchOrLinearSize), 4);
  writeInt(file, &(dds->depth), 4);
  writeInt(file, &(dds->mipMapCount), 4);

  fwrite(zero, 1, 11*4, file);
  
  writeInt(file, &(dds->pixelFormat.size), 4);
  writeInt(file, &(dds->pixelFormat.flags), 4);
  fwrite(dds->pixelFormat.id, 4, 1, file);
  writeInt(file, &(dds->pixelFormat.rgbBitCount), 4);
  writeInt(file, &(dds->pixelFormat.rBitMask), 4);
  writeInt(file, &(dds->pixelFormat.gBitMask), 4);
  writeInt(file, &(dds->pixelFormat.bBitMask), 4);
  writeInt(file, &(dds->pixelFormat.aBitMask), 4);

  writeInt(file, &(dds->caps), 4);
  writeInt(file, &(dds->caps2), 4);
  fwrite(zero, 1, 3*4, file);

  fwrite(dds->data, dds->pitchOrLinearSize, 1, file);
  fclose(file);

  return 1;
}
void FreeDdsData(Dds * dds)
{
  free(dds->data);
}
