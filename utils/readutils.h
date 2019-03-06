#ifndef READUTILS_H
#define READUTILS_H
#include <stdio.h>
#include "stdint.h"

#define MAX_IMAGE_SIZE 4096*4096*3

void readInt(FILE * file, void * data, size_t size);
void writeInt(FILE * file, void * data, size_t size);

#endif ///READUTILS_H
