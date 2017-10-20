#ifndef WAVEFILE_H
#define WAVEFILE_H
#include "string.h"


typedef struct wavefile{
String_t wavefile_tag;
String_t wavefile_name;
}wavefile;

typedef wavefile *wavefile_t;

#endif
