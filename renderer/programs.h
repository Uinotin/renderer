#ifndef PROGRAMS_H
#define PROGRAMS_H

#include <string.h>
#include "node.h"

typedef size_t (*InitProgram)(size_t * childOutDataOffsets);

typedef struct Programs
{
  InitProgram initProgram;
  UpdateProgram update;
} Programs;

void GetProgramsFromName(const char * name, Programs * programs);


#endif /// PROGRAMS_H
