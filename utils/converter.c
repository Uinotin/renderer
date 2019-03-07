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
  const char *restrict ddsBlock = (const char *)dds->data;
  for (uint32_t i = 0; i < height; i += 4) {
    dibBlock[0] = (char *)(dib->data) + width * 3 * i;
    dibBlock[1] = (char *)(dib->data) + width * 3 * (1+i);
    dibBlock[2] = (char *)(dib->data) + width * 3 * (2+i);
    dibBlock[3] = (char *)(dib->data) + width * 3 * (3+i);
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


void GenerateDdsFromDib(Dds * dds, const Dib * dib)
{
  int reverseHeight = dib->height > 0;
  dds->id[0] = 'D';
  dds->id[1] = 'D';
  dds->id[2] = 'S';
  dds->id[3] = ' ';
  dds->size = 124;
  dds->flags = 0x81007;
  dds->height = reverseHeight ? dib->height : -dib->height;
  dds->width = dib->width;
  dds->pitchOrLinearSize = dds->height * dds->width * 8 / 16;
  dds->depth = 0;
  dds->mipMapCount = 1;
  dds->pixelFormat.size = 32;
  dds->pixelFormat.flags = 0x4;
  dds->pixelFormat.id[0] = 'D';
  dds->pixelFormat.id[1] = 'X';
  dds->pixelFormat.id[2] = 'T';
  dds->pixelFormat.id[3] = '1';
  dds->pixelFormat.rBitMask = 0xff0000;
  dds->pixelFormat.gBitMask = 0xff00;
  dds->pixelFormat.bBitMask = 0xff;
  dds->pixelFormat.aBitMask = 0x0;
  dds->caps = 0x1000;
  dds->caps2 = 0x0;

  dds->data = malloc(dds->pitchOrLinearSize);
  
  uint32_t dibWidthInBytes = dds->width * 3;
  uint32_t height = dds->height;
  const uint8_t *restrict dibBlock[4];
  uint8_t *restrict ddsBlock = (uint8_t *)dds->data;
  for (uint32_t i = 0; i < height; i += 4) {
    uint32_t heightOffset = reverseHeight ? height - i - 4 : i;
    dibBlock[0] = (uint8_t *)dib->data + dibWidthInBytes * (0+heightOffset);
    dibBlock[1] = (uint8_t *)dib->data + dibWidthInBytes * (1+heightOffset);
    dibBlock[2] = (uint8_t *)dib->data + dibWidthInBytes * (2+heightOffset);
    dibBlock[3] = (uint8_t *)dib->data + dibWidthInBytes * (3+heightOffset);
    for (uint32_t j = 0; j < dibWidthInBytes; j += 4*3) {
      uint8_t colors[4][3];
      uint8_t minColor[3];
      uint8_t maxColor[3] = {0};

      uint32_t colorAverage[3] = {0};
      uint32_t minColorVal = 768;
      uint32_t maxColorVal = 0;
      for (uint32_t columnIndex = 0; columnIndex < 4; ++columnIndex)
        for (uint32_t rowIndex = 0; rowIndex < 4; ++rowIndex) {
          uint32_t dibColorVal = (uint32_t)dibBlock[columnIndex][j+rowIndex*3 + 0] +
	                    (uint32_t)dibBlock[columnIndex][j+rowIndex*3 + 1] +
	                    (uint32_t)dibBlock[columnIndex][j+rowIndex*3 + 2];
	  if (maxColorVal < dibColorVal) {
	    maxColor[0] = dibBlock[columnIndex][j+rowIndex*3 + 0];
	    maxColor[1] = dibBlock[columnIndex][j+rowIndex*3 + 1];
	    maxColor[2] = dibBlock[columnIndex][j+rowIndex*3 + 2];
	    maxColorVal = dibColorVal;
	  }
	  if (minColorVal > dibColorVal) {
	    minColor[0] = dibBlock[columnIndex][j+rowIndex*3 + 0];
	    minColor[1] = dibBlock[columnIndex][j+rowIndex*3 + 1];
	    minColor[2] = dibBlock[columnIndex][j+rowIndex*3 + 2];
	    minColorVal = dibColorVal;
	  }
          colorAverage[0] += (uint32_t)(dibBlock[columnIndex][j+rowIndex*3 + 0]);
          colorAverage[1] += (uint32_t)(dibBlock[columnIndex][j+rowIndex*3 + 1]);
	  colorAverage[2] += (uint32_t)(dibBlock[columnIndex][j+rowIndex*3 + 2]);
	}
      colorAverage[0] /= 16;
      colorAverage[1] /= 16;
      colorAverage[2] /= 16;
      colors[0][0] = (uint8_t)(((uint32_t)minColor[0] * 2 + colorAverage[0])/3);
      colors[0][1] = (uint8_t)(((uint32_t)minColor[1] * 2 + colorAverage[1])/3);
      colors[0][2] = (uint8_t)(((uint32_t)minColor[2] * 2 + colorAverage[2])/3);

      colors[1][0] = (uint8_t)(((uint32_t)maxColor[0] * 2 + colorAverage[0])/3);
      colors[1][1] = (uint8_t)(((uint32_t)maxColor[1] * 2 + colorAverage[1])/3);
      colors[1][2] = (uint8_t)(((uint32_t)maxColor[2] * 2 + colorAverage[2])/3);

      colors[2][0] = (uint8_t)(((uint16_t)colors[0][0] * 2 + (uint16_t)colors[1][0])/3);
      colors[2][1] = (uint8_t)(((uint16_t)colors[0][1] * 2 + (uint16_t)colors[1][1])/3);
      colors[2][2] = (uint8_t)(((uint16_t)colors[0][2] * 2 + (uint16_t)colors[1][2])/3);
      colors[3][0] = (uint8_t)(((uint16_t)colors[0][0] + (uint16_t)colors[1][0] * 2)/3);
      colors[3][1] = (uint8_t)(((uint16_t)colors[0][1] + (uint16_t)colors[1][1] * 2)/3);
      colors[3][2] = (uint8_t)(((uint16_t)colors[0][2] + (uint16_t)colors[1][2] * 2)/3);
      
      *((uint16_t *)ddsBlock) = ((uint16_t)colors[0][0])*31/255 << 0 |
	                        ((uint16_t)colors[0][1])*63/255 << 5 |
	                        ((uint16_t)colors[0][2])*31/255 << 11;
      *(((uint16_t *)ddsBlock)+1) = ((uint16_t)colors[1][0])*31/255 << 0 |
	                            ((uint16_t)colors[1][1])*63/255 << 5 |
	                            ((uint16_t)colors[1][2])*31/255 << 11;
      ddsBlock += 4;
      for (uint32_t columnIndex = 0; columnIndex < 4; ++columnIndex) {
	ddsBlock[reverseHeight ? 3 - columnIndex : columnIndex] = 0;
        for (uint32_t rowIndex = 0; rowIndex < 4; ++rowIndex) {
	  uint32_t smallestDifference = 1020;
	  uint8_t smallestDifferenceColorIndex;
	  for (uint8_t colorIndex = 0; colorIndex < 4; ++colorIndex) {
	    uint32_t difference = 0;
	    for (uint32_t channelIndex = 0; channelIndex < 3; ++channelIndex) {
	      int32_t channelDifference =
		(int32_t)colors[colorIndex][channelIndex]-
		(int32_t)dibBlock[columnIndex][j+rowIndex*3+channelIndex];
	      channelDifference = channelDifference >= 0 ?
		                  channelDifference : -channelDifference;
	      difference += (uint32_t)channelDifference;
	    }
	    if(difference < smallestDifference) {
	      smallestDifference = difference;
	      smallestDifferenceColorIndex = colorIndex;
	    }
	  }
	  ddsBlock[reverseHeight ? 3 - columnIndex : columnIndex] |=
	    smallestDifferenceColorIndex << (rowIndex * 2);
	}
      }
      ddsBlock += 4;
    }
  }
}
