#include "../control/pass.h"
#include "trans-tac.h"
#include "tac-main.h"

Machine_Prog_t Tac_main (Tac_Prog_t p)
{
  Pass_t typeCheck, trans;
  Machine_Prog_t q;

  trans = Pass_new ("conversion to machine",
                    VERBOSE_SUBPASS,
                    p,
                    (Poly_tyId)Trans_tac);
  q = Pass_doit (&trans);
  
  return q;
}
