#include "../lib/int.h"
#include "../control/pass.h"
#include "../control/control.h"
#include "x86.h"
#include "x86-codegen.h"
#include "peep-hole.h"
#include "x86-main.h"
Tuple_t X86_main (Machine_Prog_t p)
{
  Pass_t codeGen,
    peepHole,
    output;
  X86_Prog_t p1, p2;
  String_t f;

  codeGen = Pass_new ("codegen",
                      VERBOSE_SUBPASS,
                      p,
                      (Poly_tyId)X86_codegen);
  p1 = Pass_doit (&codeGen);

  peepHole = Pass_new ("peephole",
                       VERBOSE_SUBPASS,
                       p1,
                       (Poly_tyId)PeepHole_shrink);
  p2 = Pass_doit (&peepHole);

  p1 = 0;
  return Tuple_new (p2, 0);
}
