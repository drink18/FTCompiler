#ifndef COMPILE_H
#define COMPILE_H

#include "../lib/linkedList.h"




extern TCCState *s;

int get_line_sub();
LinkedList_t Compile_compile (LinkedList_t files);

#endif
