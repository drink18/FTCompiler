#include "../lib/string.h"
#include "../lib/file.h"
#include "../lib/tuple.h"
#include "../lib/wavefile.h"
#include "../lib/error.h"
#include "../main/main-main.h"
#include "../control/command-line.h"
#include "../control/pass.h"
#include "../parser/parse.h"
#include "../ast/ast.h"
#include "../ast/x86_c.h"
#include "../elaborate/elaborate-main.h"
#include "../hil/hil.h"
#include "../hil/hil-main.h"
#include "../tac/tac-main.h"
#include "../machine/machine-main.h"

//firstech hardware platform
#include "../firstech/x86.h"
#include "../firstech/x86-main.h"
#include "../firstech/firstech_code_gen.h"
#include "compile.h"
#include <string.h>
#include <windows.h>
LinkedList_t wave_file_list=0;
static String_t Compile_one (String_t file);

static String_t Compile_one (String_t rawfile)
{
  int pathsize=0,i=0,ret;
  Pass_t lexAndPass, elaborate,
    flatten, tacPass, machinePass, x86Pass;
  Ast_Prog_t ast;
  Hil_Prog_t hil;
  Tac_Prog_t tac;
  Machine_Prog_t machine;
  Tuple_t tuple;
  X86_Prog_t x86;
  //strip the path info
  String_t filename=String_substr(rawfile,'.');

  ret = tcc_open(s, rawfile);
  if (ret < 0) {
            tcc_error_noabort("file '%s' not found", filename);
        return ret;
    }



  //init wave file list
  wave_file_list=LinkedList_new();
  if (Control_dump_lookup (DUMP_CPP)){
  	/* open the file */
	String_t cppFileName=genFileName (rawfile, "cpp");

	s->ppfp = fopen(cppFileName, "w");
    if (!s->ppfp)
                tcc_error("could not write '%s'", cppFileName);
	tcc_preprocess(s);

	fclose(s->ppfp);
     return 0;
  }
   preprocess_init(s);
  lexAndPass = Pass_new ("lexAndParse",
                         VERBOSE_SUBPASS,
                         rawfile,
                         (Poly_tyId)Parse_parse);

  ast = Pass_doit (&lexAndPass);
  tcc_close();
 
  #ifdef _DEBUG
  if (Control_dump_lookup (DUMP_AST)){
    File_saveToFile (genFileName (rawfile, "ast"),
                     (Poly_tyPrint)Ast_Prog_print, ast);
  }
  #endif
     // File_saveToFile (genFileName (rawfile, "c"),
     //                (Poly_tyPrint)X86_C_Prog_print, ast);



  elaborate = Pass_new ("elaboration",
                        VERBOSE_SUBPASS,
                        ast,
                        (Poly_tyId)Elaborate_main);
  hil = Pass_doit (&elaborate);
  #ifdef _DEBUG
  if (Control_dump_lookup (DUMP_HIL)){
    File_saveToFile (genFileName (rawfile, "hil"),
                     (Poly_tyPrint)Hil_Prog_print, hil);
  }
  #endif
  ast = 0;

  flatten = Pass_new ("hil",
                      VERBOSE_SUBPASS,
                      hil,
                      (Poly_tyId)Hil_main);
  tac = Pass_doit (&flatten);
  #ifdef _DEBUG
  if (Control_dump_lookup (DUMP_TAC)){
    File_saveToFile (genFileName (rawfile, "tac"),
                     (Poly_tyPrint)Tac_Prog_print, tac);
  }
  #endif
  hil = 0;

  tacPass = Pass_new ("tac",
                      VERBOSE_SUBPASS,
                      tac,
                      (Poly_tyId)Tac_main);
  machine = Pass_doit (&tacPass);
  #ifdef _DEBUG
  if (Control_dump_lookup (DUMP_MACHINE)){
    File_saveToFile (genFileName (rawfile, "machine"),
                     (Poly_tyPrint)Machine_Prog_print,
                     machine);
  }
  #endif
 
  x86Pass = Pass_new ("x86",
                      VERBOSE_SUBPASS,
                      machine,
                      (Poly_tyId)X86_main);
  tuple = Pass_doit (&x86Pass);
  x86 = Tuple_first (tuple);
  if (Control_dump_lookup (DUMP_X86)){
    File_saveToFile (genFileName (rawfile, "s"),
                     (Poly_tyPrint)Firstech_Prog_print, x86);
  }
  if(Control_dump_lookup (DUMP_PAR))
  	{
	Firstech_Code_gen(x86,filename,0);
  	}
  else  Firstech_Code_gen(x86,filename,1);
  
  //add for delete
  tcc_delete(s);	
  return Tuple_second (tuple);
}

LinkedList_t Compile_compile (LinkedList_t files)
{
  return LinkedList_map (files, (Poly_tyId)Compile_one);
}
