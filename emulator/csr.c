
#include "glb.h"
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include "utils.h"
#include "csr.h"

i32 csr[4096];

void csr_init()
{
  csr_reg_m_reset(csr);
}
