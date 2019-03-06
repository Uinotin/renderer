#include "readutils.h"
#include <string.h>

int32_t endianChecker = 1;
#define IS_BIG_ENDIAN (((char *)&endianChecker)[0] == 0)


void readInt(FILE * file, void * data, size_t size)
{
  char *i, *j;
  fread(data, size, 1, file);
  if (IS_BIG_ENDIAN)
    for (i = (char *)data, j = i + size - 1; i < j; ++i,--j) {
      char temp = *i;
      *i = *j;
      *j = temp;
    }
}

void writeInt(FILE * file, void * data, size_t size)
{
  char store[8];
  memcpy(store, data, size);
  char *i, *j;
  if (IS_BIG_ENDIAN)
    for (i = (char *)store, j = i + size - 1; i < j; ++i,--j) {
      char temp = *i;
      *i = *j;
      *j = temp;
    }
  fwrite((const void*)store, size, 1, file);
}
