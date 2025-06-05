#include "tools.h"

void TSG_INIT(void)
{
    TSG_CNTCR = 0;
    TSG_CNTCVL = 0;
    TSG_CNTCVU = 0;
    TSG_CNTFID0 = 240000000;
    TSG_CNTCR = 1;
}
