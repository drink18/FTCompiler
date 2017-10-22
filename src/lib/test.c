#include <stdio.h>
#include "test.h"
#include "error.h"

#include "property-list.h"
#include "property.h"
#include "../atoms/id.h"
#include "poly.h"
#include "box.h"


void print (String_t s)
{
  printf ("%s", s);
}

int Lib_test ()
{
  Box_t box = Box_v 
    (Box_str ("aaaaaaaaaaaaaaaaa"),
     Box_indent (Box_v (Box_str ("b"),
                        Box_str ("c"),
                        0), 4),
     Box_str ("d"),
     0);
  box = Box_h (box, Box_str ("x"), 0);
  Box_print (box, print);
  return 0;
}
