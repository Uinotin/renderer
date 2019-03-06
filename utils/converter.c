#include <stdlib.h>
#include <string.h>
#include "converter.h"

void GenerateDibFromDds(Dib * dib, const Dds * dds)
{
  dib->headerSize = 40;
  dib->width = (int32_t)(dds->width);
  dib->height = -((int32_t)(dds->height));
  dib->colorPlanes = 1;
  dib->compression = 0;
  dib->bitsPerPixel = 24;
  dib->resolutionX = 2835;
  dib->resolutionY = 2835;
  dib->numColors = 0;
  dib->numImportantColors = 0;

  dib->imageSize = dds->width * dds->height * 3;
  dib->data = malloc(dib->imageSize);

  uint32_t width = dds->width;
  uint32_t height = dds->height;
  char *restrict dibBlock[4];
  const char *restrict ddsBlock = dds->data;
  for (uint32_t i = 0; i < height; i += 4) {
    dibBlock[0] = (char *)dib->data + width * 3 * i;
    dibBlock[1] = (char *)dib->data + width * 3 * (1+i);
    dibBlock[2] = (char *)dib->data + width * 3 * (2+i);
    dibBlock[3] = (char *)dib->data + width * 3 * (3+i);
    for (uint32_t j = 0; j < width; j += 4, ddsBlock += 8) {
      /// todo: Take into account endiannes
      uint16_t colorCompressed0 = *((uint16_t *)ddsBlock);
      uint16_t colorCompressed1 = *((uint16_t *)(ddsBlock+2));
      uint32_t colorIndices = *((uint32_t *)(ddsBlock+4));
      char colors[4][3];
      colors[0][0] = (colorCompressed0 & 0x1f)*0xff/0x1f;
      colors[0][1] = (colorCompressed0 >> 5 & 0x3f)*0xff/0x3f;
      colors[0][2] = (colorCompressed0 >> 11 & 0x1f)*0xff/0x1f;
      colors[1][0] = (colorCompressed1 & 0x1f)*0xff/0x1f;
      colors[1][1] = (colorCompressed1 >> 5 & 0x3f)*0xff/0x3f;
      colors[1][2] = (colorCompressed1 >> 11 & 0x1f)*0xff/0x1f;
      colors[2][0] = (char)(((uint16_t)colors[0][0] * 2 + (uint16_t)colors[1][0])/3);
      colors[2][1] = (char)(((uint16_t)colors[0][1] * 2 + (uint16_t)colors[1][1])/3);
      colors[2][2] = (char)(((uint16_t)colors[0][2] * 2 + (uint16_t)colors[1][2])/3);
      colors[3][0] = (char)(((uint16_t)colors[0][0] + (uint16_t)colors[1][0] * 2)/3);
      colors[3][1] = (char)(((uint16_t)colors[0][1] + (uint16_t)colors[1][1] * 2)/3);
      colors[3][2] = (char)(((uint16_t)colors[0][2] + (uint16_t)colors[1][2] * 2)/3);

      for (int columnIndex = 0; columnIndex < 4; ++columnIndex)
        for (int rowIndex = 0; rowIndex < 4; ++rowIndex) {
	  char colorIndex = (char)(colorIndices & 0x3);
	  colorIndices = colorIndices >> 2;
	  memcpy(dibBlock[columnIndex], colors[colorIndex], 3);
	  dibBlock[columnIndex] += 3;
	}
    }
  }
}
