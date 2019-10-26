
#include "glb.h"
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include "utils.h"
#include "csr.h"

u32 csr[4096];

void __attribute__((constructor)) csr_init()
{
  csr_reg_m_reset(csr);
}
