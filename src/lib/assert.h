#ifndef ASSERT_H
#define ASSERT_H

#include <stdio.h>
#include <stdlib.h>

extern int Assert_flag;

/* it seems that the standard assert does not work,
 * so I had to make my own.
 */

#ifdef DEBUG

#define Assert_ASSERT(e)                                \
  do {                                                  \
    if (Assert_flag){                                   \
      if (e)                                            \
        ;                                               \
      else{                                                   \
        fprintf (stderr,                                      \
                 "assertion failed: %s %d: %s\n",             \
                 __FILE__, __LINE__, #e);                     \
        exit (1);                                             \
      }                                                       \
    }                                                         \
  }while (0)

#else

#define Assert_ASSERT(e)

#endif

#endif
