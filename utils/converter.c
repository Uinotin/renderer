#include <stdlib.h>
#include <string.h>
#include "converter.h"

static void Uint16Vec3Lerp(const uint16_t * color0, const uint16_t * color1,
			   uint16_t * outColor, uint16_t weight, uint16_t maxWeight)
{
  outColor[0] = ((maxWeight - weight)*color0[0] + weight*color1[0])/maxWeight;
  outColor[1] = ((maxWeight - weight)*color0[1] + weight*color1[1])/maxWeight;
  outColor[2] = ((maxWeight - weight)*color0[2] + weight*color1[2])/maxWeight;
}

static void Int32Vec3Lerp(const int32_t * color0, const int32_t * color1,
			  int32_t * outColor, int32_t weight, int32_t maxWeight)
{
  outColor[0] = ((maxWeight - weight)*color0[0] + weight*color1[0])/maxWeight;
  outColor[1] = ((maxWeight - weight)*color0[1] + weight*color1[1])/maxWeight;
  outColor[2] = ((maxWeight - weight)*color0[2] + weight*color1[2])/maxWeight;
}

/// This function converts a DXT1-compressed DDS
/// into a DIB (BMP file)
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
  uint8_t * dibBlock[4];
  const uint8_t * ddsBlock = (const uint8_t *)dds->data;
  for (uint32_t i = 0; i < height; i += 4) {
    dibBlock[0] = (uint8_t *)(dib->data) + width * 3 * i;
    dibBlock[1] = (uint8_t *)(dib->data) + width * 3 * (1+i);
    dibBlock[2] = (uint8_t *)(dib->data) + width * 3 * (2+i);
    dibBlock[3] = (uint8_t *)(dib->data) + width * 3 * (3+i);
    for (uint32_t j = 0; j < width; j += 4, ddsBlock += 8) {
      /// todo: Take into account endiannes
      uint16_t colorCompressed0 = *((uint16_t *)ddsBlock);
      uint16_t colorCompressed1 = *((uint16_t *)(ddsBlock+2));
      const uint8_t * colorIndices = ddsBlock+4;
      uint16_t colors[4][3];
      colors[0][0] = (colorCompressed0 & 0x1f)*0xff/0x1f;
      colors[0][1] = (colorCompressed0 >> 5 & 0x3f)*0xff/0x3f;
      colors[0][2] = (colorCompressed0 >> 11 & 0x1f)*0xff/0x1f;
      colors[1][0] = (colorCompressed1 & 0x1f)*0xff/0x1f;
      colors[1][1] = (colorCompressed1 >> 5 & 0x3f)*0xff/0x3f;
      colors[1][2] = (colorCompressed1 >> 11 & 0x1f)*0xff/0x1f;
      Uint16Vec3Lerp(colors[0], colors[1], colors[2], 1, 3);
      Uint16Vec3Lerp(colors[0], colors[1], colors[3], 2, 3);

      for (uint8_t columnIndex = 0; columnIndex < 4; ++columnIndex, ++colorIndices)
        for (uint8_t rowIndex = 0; rowIndex < 4; ++rowIndex, dibBlock[columnIndex] += 3) {
	  uint8_t colorIndex = (*colorIndices >> (2 * rowIndex)) & 0x3;
	  dibBlock[columnIndex][0] = (uint8_t)colors[colorIndex][0];
	  dibBlock[columnIndex][1] = (uint8_t)colors[colorIndex][1];
	  dibBlock[columnIndex][2] = (uint8_t)colors[colorIndex][2];
	}
    }
  }
}

static int32_t Int32Vec3DistanceSquared(const int32_t * color0, const int32_t * color1)
{
  int32_t distanceSquared = 0;
  for (int i = 0; i < 3; ++i) {
    int32_t channelDiff = color0[i] - color1[i];
    distanceSquared += channelDiff * channelDiff;
  }

  return distanceSquared;
}


/// This function compresses an image in DIB format(BMP file)
/// into a DDS file.
void GenerateDdsFromDib(Dds * dds, const Dib * dib)
{
  int reverseHeight = dib->height > 0;
  /// Write header information
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
  dds->pixelFormat.rgbBitCount = 0x0;
  dds->pixelFormat.rBitMask = 0x0;
  dds->pixelFormat.gBitMask = 0x0;
  dds->pixelFormat.bBitMask = 0x0;
  dds->pixelFormat.aBitMask = 0x0;
  dds->caps = 0x1000;
  dds->caps2 = 0x0;

  dds->data = malloc(dds->pitchOrLinearSize);

  uint32_t dibWidthInBytes = dds->width * 3;
  uint32_t height = dds->height;
  const uint8_t * dibBlock[4];
  uint8_t * ddsBlock = (uint8_t *)dds->data;

  /// The algorithm goes through every 4x4 block in the texture
  /// As of current, the algorithm does not take into account neighbouring blocks.
  /// In many cases seeing the bigger picture would help make the right choices,
  /// but unfortunately, time is limited.
  for (uint32_t i = 0; i < height; i += 4) {
    uint32_t heightOffset = reverseHeight ? height - i - 4 : i;

    dibBlock[0] = (uint8_t *)dib->data + dibWidthInBytes * (0+heightOffset);
    dibBlock[1] = (uint8_t *)dib->data + dibWidthInBytes * (1+heightOffset);
    dibBlock[2] = (uint8_t *)dib->data + dibWidthInBytes * (2+heightOffset);
    dibBlock[3] = (uint8_t *)dib->data + dibWidthInBytes * (3+heightOffset);
    for (uint32_t j = 0; j < dibWidthInBytes; j += 4*3) {
      int32_t blockColors[16*3];
      for (uint32_t columnIndex = 0; columnIndex < 4; ++columnIndex)
        for (uint32_t rowIndex = 0; rowIndex < 4; ++rowIndex)
	  for (uint32_t channelIndex = 0; channelIndex < 3; ++channelIndex)
	    blockColors[3*(columnIndex * 4 + rowIndex) + channelIndex] =
	      (int32_t)dibBlock[columnIndex][j + rowIndex * 3 + channelIndex];

      int32_t * colors = blockColors;
      int32_t finalBlockColors[2][3];
      int8_t finalClosestColorIndex[16];
      int32_t finalLeastSquares = 0xffffff;

      /// Go through each two-pixel combination in the block to see
      /// if their colour values as the block colour values
      /// would give the smallest sum of square distances from
      /// block's pixels' colour values to their respective closest
      /// block color value. This doesn't always produce the most
      /// optimal solution but given that at least two of the pixels
      /// always gets an exact colour match, it's in most cases
      /// the best solution.
      /// To improve this algorithm, one could also evaluate all pixel
      /// colours as possible middle colours instead of just as
      /// the extreme ones, but it is
      for (int32_t colorItr0 = 0; colorItr0 < 15*3; colorItr0 += 3)
	for (int32_t colorItr1 = colorItr0 + 3; colorItr1 < 16*3; colorItr1 += 3) {
        int8_t closestColorIndex[16];
	int32_t middleColor0[3];
	int32_t middleColor1[3];
	Int32Vec3Lerp(colors + colorItr0, colors + colorItr1, middleColor0, 1, 3);
	Int32Vec3Lerp(colors + colorItr0, colors + colorItr1, middleColor1, 2, 3);
	int32_t combinedLeastSquares = 0;
        for (uint32_t colorIndex = 0; colorIndex < 16; ++colorIndex) {
	  int32_t * currentColor = blockColors + colorIndex * 3;
          int32_t smallestSquare = 0xffff;

	  int32_t squareDistance = Int32Vec3DistanceSquared(currentColor,
					                    colors + colorItr0);
	  if (squareDistance < smallestSquare) {
	    smallestSquare = squareDistance;
	    closestColorIndex[colorIndex] = 0;
	  }
	  squareDistance = Int32Vec3DistanceSquared(currentColor,
					            colors + colorItr1);
	  if (squareDistance < smallestSquare) {
	    smallestSquare = squareDistance;
	    closestColorIndex[colorIndex] = 1;
	  }
	  squareDistance = Int32Vec3DistanceSquared(currentColor,
					            middleColor0);
	  if (squareDistance < smallestSquare) {
	    smallestSquare = squareDistance;
	    closestColorIndex[colorIndex] = 2;
	  }
	  squareDistance = Int32Vec3DistanceSquared(currentColor,
					            middleColor1);
	  if (squareDistance < smallestSquare) {
	    smallestSquare = squareDistance;
	    closestColorIndex[colorIndex] = 3;
	  }
	  combinedLeastSquares += smallestSquare;
	}
	if (combinedLeastSquares < finalLeastSquares) {
	  finalLeastSquares = combinedLeastSquares;
	  memcpy(finalBlockColors[0], colors+colorItr0, 3*sizeof(int32_t));
	  memcpy(finalBlockColors[1], colors+colorItr1, 3*sizeof(int32_t));
	  memcpy(finalClosestColorIndex, closestColorIndex, 16);
	}
      }
      /// Write the final colours to the block
      uint16_t * ddsBlockColors = (uint16_t *)ddsBlock;
      ddsBlockColors[0] = ((uint16_t)finalBlockColors[0][0])*31/255 << 0 |
	                  ((uint16_t)finalBlockColors[0][1])*63/255 << 5 |
	                  ((uint16_t)finalBlockColors[0][2])*31/255 << 11;
      ddsBlockColors[1] = ((uint16_t)finalBlockColors[1][0])*31/255 << 0 |
	                  ((uint16_t)finalBlockColors[1][1])*63/255 << 5 |
	                  ((uint16_t)finalBlockColors[1][2])*31/255 << 11;
      /// Make sure that first color in the block has a higher value as uint16_t
      if (ddsBlockColors[0] < ddsBlockColors[1]) {
	uint16_t temp = ddsBlockColors[0];
	ddsBlockColors[0] = ddsBlockColors[1];
	ddsBlockColors[1] = temp;
	/// Change finalClosestColorIndex zeros to ones and threes to fours
	/// and vice versa
	for (uint32_t colorIndex = 0; colorIndex < 16; ++colorIndex)
	  finalClosestColorIndex[colorIndex] +=
	    -2 * ( finalClosestColorIndex[colorIndex] % 2) + 1;
      } else if (ddsBlockColors[0] == ddsBlockColors[1]) {
	/// If the colour values are the same, let's change
	/// the second colour value's blue channel value
	/// by one. As in most cases the algorithm
	/// ignores the second colour if both are the same,
	/// it is fairly unlikely for it to matter a great deal.
	/// If the blue channel value equals 0, add 1 to the first
	/// colour blue channel instead.
	if (ddsBlockColors[1] & 0x1f)
	  --(ddsBlockColors[1]);
	else ++(ddsBlockColors[0]);
      }
      ddsBlock += 4;
      /// Write the pixel colour indices to the block
      for (uint32_t columnIndex = 0; columnIndex < 4; ++columnIndex) {
	ddsBlock[reverseHeight ? 3 - columnIndex : columnIndex] = 0;
        for (uint32_t rowIndex = 0; rowIndex < 4; ++rowIndex)
	  ddsBlock[reverseHeight ? 3 - columnIndex : columnIndex] |=
	    finalClosestColorIndex[columnIndex*4 + rowIndex] << (rowIndex * 2);
      }
      ddsBlock += 4;
    }
  }
}
