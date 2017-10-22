#include "../control/pass.h"
#include "hil.h"
#include "dead-code.h"
#include "../tac/tac.h"
#include "trans-hil.h"
#include "hil-main.h"

Tac_Prog_t Hil_main (Hil_Prog_t p)
{
  Pass_t deadCode, trans;
  Hil_Prog_t p1;
  Tac_Prog_t q;

  deadCode = Pass_new ("dead code",
                       VERBOSE_SUBPASS,
                       p,
                       (Poly_tyId)Hil_deadCode);
  p1 = Pass_doit (&deadCode);  

  trans = Pass_new ("conversion to tac",
                    VERBOSE_SUBPASS,
                    p1,
                    (Poly_tyId)Hil_trans);
  q = Pass_doit (&trans);

  return q;
}
