#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../lib/assert.h"
#include "../lib/mem.h"
#include "../lib/linkedList.h"
#include "../lib/string.h"
#include "../lib/file.h"
#include "../lib/mem.h"
#include "../lib/int.h"
#include "../lib/io.h"
#include "../lib/gc.h"
#include "../lib/hash.h"
#include "../control/command-line.h"
#include "../control/pass.h"
#include "../main/compile.h"
#include "main-main.h"
#define  P2F_VERSION  "2.2.1"
//2.0.1_rc
//2.1.0   add  export grad wave tx wave function 
//		 import tx wave
//		 	-import  TX   CH_NO BRAM_NO wavepath wavename
//		 import grad wave
//		       -import  GRAD   wavepath
//		 change the par file's src file path
//2.2.0
// 1.support Array
//2.2.1
// support output file func.
static char report_name[60];
void GenReportName()
{
	time_t curtime = time(0);
	struct tm *time;
	time=localtime(&curtime);
	int sec, min, day, mon, year, hour;

	year = time->tm_year;
	mon = time->tm_mon;
	day = time->tm_mday;
	min = time->tm_min;
	hour = time->tm_hour;
	sec = time->tm_sec;
	snprintf(report_name, 50, "report_%d.%d.%d.%d.%d.%d.txt", year + 1900, mon + 1, day, hour, min, sec);
}

static void help ()
{
	printf("P2F version " P2F_VERSION " -  Firstech SEQ Compiler - Copyright (C) 2016-2020\n"
			   "Usage: p2f [options...]  infile(s)...\n"
			   "General options:\n"
			   "  -o outfile  set output filename\n"
			   "Preprocessor options:\n"
			   "  -Idir 	  add include path 'dir'\n"
			   "  -D sym[=val] define 'sym' with value 'val'\n"
			   "  -U sym 	  undefine 'sym'\n"
			   "Debug options:\n"

			   "  -dump [cpp|par|asm]   dump file with dedicate type'\n"
			   "   	Type description\n"
			   "	cpp   file after preprocess\n"
			   "	asm	  assemble file 	\n"
			   );



}
TCCState *s;

int Main_main (int argc, char **argv)
{
  Pass_t mainp;
  LinkedList_t files;

  Mem_init ();
  printf("P2F version " P2F_VERSION " -  Firstech SEQ Compiler - Copyright (C) 2016-2020\n");
  if (argc<2){
    help ();
    return 0;
  }
  s = tcc_new();
  if (!s->nostdinc) {
        /* default include paths */
        /* -isystem paths have already been handled */
        tcc_add_sysinclude_path(s, CONFIG_TCC_SYSINCLUDEPATHS);
    }
  files = CommandLine_doarg (--argc, ++argv);

  if (LinkedList_isEmpty (files))
    return 0;

  mainp = Pass_new ("compile",
                    VERBOSE_PASS,
                    files,
                    (Poly_tyId)Compile_compile);
  Pass_doit (&mainp);

  if (Control_order (VERBOSE_DETAIL, Control_verbose)){
  	GenReportName();
  	FILE *report=fopen(report_name,"w");
    Mem_status (report);
    Hash_statusAll (report);
	fclose(report);
  }

  return 0;
}


