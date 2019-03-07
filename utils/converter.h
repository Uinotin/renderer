#ifndef CONVERTER_H
#define CONVERTER_H

#include "dds.h"
#include "bmp.h"

void GenerateDibFromDds(Dib * dib, const Dds * dds);
void GenerateDdsFromDib(Dds * dds, const Dib * dib);

#endif /// CONVERTER_H
