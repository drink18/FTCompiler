#ifndef ERROR_MSG_H
#define ERROR_MSG_H

#include "../lib/string.h"
#include "region.h"

void ErrorMsg_init ();
void ErrorMsg_errorExit ();
void ErrorMsg_lexError (char *msg, 
                        char *fname, 
                        int line, 
                        int column);
void ErrorMsg_syntaxError (char *msg, Region_t r);
void ErrorMsg_elabError (char *msg, Region_t r);
void WarningMsg_elabError (char *s, Region_t r);


#endif
