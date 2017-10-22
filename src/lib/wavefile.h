#ifndef WAVEFILE_H
#define WAVEFILE_H
#include "string.h"

typedef struct wavefile{
String_t wavefile_tag;
int bram_no;
int ch_no;
String_t wavename;
String_t wavefile_name;
}wavefile;

typedef wavefile *wavefile_t;

#endif
