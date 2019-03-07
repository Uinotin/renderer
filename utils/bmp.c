#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "bmp.h"

typedef struct BmpHeader
{
  char id[2];
  uint32_t size;
  uint32_t offsetToData;
} BmpHeader;

static int SanityCheck(BmpHeader * header, Dib * dib)
{
  printf("File size: %u\nData offset: %u\nInfo header size: %u\nDimensions: %ix%i\nColor planes: %u\nBits per pixel: %u\nCompression type: %u\nImage size: %u\nResolution: %ux%u\nNumber of colors: %u\nNumber of important colors: %u\n",
	 header->size, header->offsetToData, dib->headerSize,
	 dib->width, dib->height, dib->colorPlanes, dib->bitsPerPixel,
	 dib->compression, dib->imageSize, dib->resolutionX, dib->resolutionY,
	 dib->numColors, dib->numImportantColors);
  if (header->id[0] != 'B' || header->id[1] != 'M') {
    printf("Error: File is not a BMP file.\nHeader field: \"%c%c\", expected: \"BM\"\n",
	   header->id[0], header->id[1]);
    return 0;
  }
  
  if(dib->bitsPerPixel != 24) {
    printf("Error: Bits per pixel is: %u\nOnly 24bpp files are supported\n",
	   dib->bitsPerPixel);
    return 0;
  }
  if (dib->imageSize > header->size - header->offsetToData) {
    printf("Error: Image size is bigger than the file size\n");
    return 0;
  }
  if (dib->imageSize > MAX_IMAGE_SIZE) {
    printf("Error: Image size is too big\n");
    return 0;
  }
  if (dib->headerSize != 108 && dib->headerSize != 40) {
    printf("Error: Unsupported header size\n");
    return 0;
  }
  return 1;
}

int LoadBmp(Dib * dib, const char * filename)
{
  FILE * file = fopen(filename, "rb");
  if (!file) {
    printf("Error: Could not open file %s\n", filename);
    return 0;
  }
  BmpHeader bmpHeader;
  readInt(file, bmpHeader.id, 2);
  readInt(file, &(bmpHeader.size), 4);
  readInt(file, &(bmpHeader.offsetToData), 4);
  readInt(file, &(bmpHeader.offsetToData), 4);
  readInt(file, &(dib->headerSize), 4);
  readInt(file, &(dib->width), 4);
  readInt(file, &(dib->height), 4);
  readInt(file, &(dib->colorPlanes), 2);
  readInt(file, &(dib->bitsPerPixel), 2);
  readInt(file, &(dib->compression), 4);
  readInt(file, &(dib->imageSize), 4);
  readInt(file, &(dib->resolutionX), 4);
  readInt(file, &(dib->resolutionY), 4);
  readInt(file, &(dib->numColors), 4);
  readInt(file, &(dib->numImportantColors), 4);

  if(dib->headerSize == 108) {
    fseek(file, 16, SEEK_CUR);
    fread(dib->colorSpaceType, 4, 1, file);
    fread(dib->colorSpaceEndpointData, 36, 1, file);
    readInt(file, &(dib->redGamma), 4);
    readInt(file, &(dib->greenGamma), 4);
    readInt(file, &(dib->blueGamma), 4);
  }

  if (!SanityCheck(&bmpHeader, dib)) {
    fclose(file);
    return 0;
  }
  dib->data = (char *)malloc(dib->imageSize);
  fseek(file, bmpHeader.offsetToData, SEEK_SET);
  fread(dib->data, dib->imageSize, 1, file);
  fclose(file);
  return 1;
}

int WriteBmp(Dib * dib, const char * filename)
{
  if (access(filename, F_OK) != -1) {
    printf("Error: Output file already exists\n");
    return 0;
  }
  char zero = 0;
  BmpHeader bmpHeader;
  bmpHeader.id[0] = 'B';
  bmpHeader.id[1] = 'M';
  bmpHeader.offsetToData = 14 + dib->headerSize;
  bmpHeader.size = bmpHeader.offsetToData + dib->imageSize;
  if (!SanityCheck(&bmpHeader, dib))
    return 0;
  FILE * file = fopen(filename, "wb");
  if(!file) {
    printf("Error: Unable to write file %s\n", filename);
    return 0;
  }
  writeInt(file, bmpHeader.id, 2);
  writeInt(file, &(bmpHeader.size), 4);
  fwrite(&zero, 1, 4, file);
  writeInt(file, &(bmpHeader.offsetToData), 4);
  writeInt(file, &(dib->headerSize), 4);
  writeInt(file, &(dib->width), 4);
  writeInt(file, &(dib->height), 4);
  writeInt(file, &(dib->colorPlanes), 2);
  writeInt(file, &(dib->bitsPerPixel), 2);
  writeInt(file, &(dib->compression), 4);
  writeInt(file, &(dib->imageSize), 4);
  writeInt(file, &(dib->resolutionX), 4);
  writeInt(file, &(dib->resolutionY), 4);
  writeInt(file, &(dib->numColors), 4);
  writeInt(file, &(dib->numImportantColors), 4);

  if(dib->headerSize == 108) {
    fseek(file, 16, SEEK_CUR);
    fwrite(&zero, 1, 16, file);
    fwrite(dib->colorSpaceType, 4, 1, file);
    fwrite(dib->colorSpaceEndpointData, 36, 1, file);
    writeInt(file, &(dib->redGamma), 4);
    writeInt(file, &(dib->greenGamma), 4);
    writeInt(file, &(dib->blueGamma), 4);
  }
  fseek(file, bmpHeader.offsetToData, SEEK_SET);
  fwrite(dib->data, dib->imageSize, 1, file);
  fclose(file);
  return 1;
}

void FreeDib(Dib * dib)
{
  free(dib->data);
}
