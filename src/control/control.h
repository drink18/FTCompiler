#ifndef CONTROL_H
#define CONTROL_H

#include "../lib/linkedList.h"
#include "../lib/string.h"

typedef enum {VERBOSE_SILENT, 
              VERBOSE_PASS, 
              VERBOSE_SUBPASS, 
              VERBOSE_DETAIL} Verbose_t;
typedef enum {EXPERT_NORMAL, 
              EXPERT_EXPERT} Expert_t;
typedef enum {DUMP_AST,
			  DUMP_CPP,
              DUMP_HIL,
              DUMP_TAC,
              DUMP_MACHINE,
              DUMP_PAR,
              DUMP_X86} Dump_t;

extern int Control_bufferSize;
extern Expert_t Control_expert;
extern int Control_showType;
extern LinkedList_t Control_trace;
extern Verbose_t Control_verbose;
extern String_t Control_o;

/* this is platform dependent, so should be 
 * sent to another place.
 */
extern String_t Control_asmDirectory;
extern String_t Control_libDirectory;

int Control_order (int l1, int l2);
void Control_dump_insert (Dump_t);
int Control_dump_lookup (Dump_t);
int Control_mayDropPass (String_t name);
void Control_dropPass_insert (String_t name);
void Control_init ();
void Control_printFlags ();


#endif
