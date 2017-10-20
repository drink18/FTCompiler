#include "mem.h"
#include "wavefile.h"
#define W wavefile_t
W wavefile_new_grad(String_t name)
{
	W  e;
	Mem_NEW (e);
	e->wavefile_tag = "GRAD";
	e->wavefile_name = name;
	return e;
}
W wavefile_new_rf(String_t name,int bram_no,int ch_no,String_t wavename)
{
	W  e;
	Mem_NEW (e);
	e->wavefile_tag = "TX";
	e->bram_no=bram_no;
	e->ch_no=ch_no;
	e->wavename=wavename;
	e->wavefile_name = name;
	return e;
}


#undef  W
