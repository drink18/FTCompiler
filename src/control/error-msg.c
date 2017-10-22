#include <stdio.h>
#include <stdlib.h>
#include "../lib/assert.h"
#include "error-msg.h"

static int numErrors = 0;
static int numWarn=0;


void ErrorMsg_syntaxError (char *s, Region_t r)
{
  Assert_ASSERT (r);  
  numErrors++;
  
  fprintf (stderr, "%s:%d: syntax error: %s\n",Id_toString(r->filename),r->line,s);
  exit (0);
}

void ErrorMsg_elabError (char *s, Region_t r)
{
  Assert_ASSERT (s);
  numErrors++;
  if (r)
    fprintf (stderr, "%s:%d: error: %s\n",Id_toString(r->filename),r->line,s);
  else 
    fprintf (stderr, "error: %s\n",s);

}
void WarningMsg_elabError (char *s, Region_t r)
{
  Assert_ASSERT (s);
  
  if (r)
    fprintf (stderr, "%s:%d: warning: %s\n",Id_toString(r->filename),r->line,s);
  else 
    fprintf (stderr, "%s:%s: warning: %s\n",s);
}

void ErrorMsg_CodegenError (char *s)
{
  Assert_ASSERT (s);  
  numErrors++;
  fprintf (stderr, "Code gen error: %s\n",           
           s);
  exit (0);
}

void ErrorMsg_init ()
{
  numErrors = 0;
}

void ErrorMsg_errorExit ()
{
  if (numErrors){
    numErrors = 0;
    exit (0);
  }
}

