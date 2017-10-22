#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include "string.h"
#include "poly.h"

#define T File_t

typedef FILE *T;
typedef void (*Poly_tyPrint)(Poly_t, T);
typedef void (*Poly_tyPrintFname)(Poly_t, T,String_t);

int File_saveToFile (String_t fname, 
                     Poly_tyPrint print,
                     Poly_t x);
int File_saveToFileFname (String_t fname, 
                     Poly_tyPrintFname print,
                     Poly_t x,String_t rawfile);


#undef T

#endif
