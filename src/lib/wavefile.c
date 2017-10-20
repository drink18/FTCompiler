#include "mem.h"
#include "wavefile.h"
#define W wavefile_t
W wavefile_new(String_t tag, String_t name)
{
	W  e;
	Mem_NEW (e);
	e->wavefile_tag = tag;
	e->wavefile_name = name;
	return e;
}


#undef  W
