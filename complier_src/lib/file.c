#include "error.h"
#include "file.h"

#define T File_t

int File_saveToFile (String_t fname, 
                     void (*print)(Poly_t, T),
                     Poly_t x)
{
  T file = fopen (fname, "w+");
  if (!file)
    Error_bug (String_concat 
               ("fail to open file: ",
                fname, 
                0));
  print (x, file);
  fclose (file);  
  return 0;
}
int File_saveToFileFname (String_t fname, 
                     void (*print)(Poly_t, T,String_t fname),
                     Poly_t x,String_t rawfile)
{
  T file = fopen (fname, "w+");
  if (!file)
    Error_bug (String_concat 
               ("fail to open file: ",
                fname, 
                0));
  print (x, file,rawfile);
  fclose (file);  
  return 0;
}


#undef T
