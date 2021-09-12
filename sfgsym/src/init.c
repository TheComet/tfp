#include "sfgsym/init.h"
#include "cstructures/init.h"

/* ------------------------------------------------------------------------- */
int
sfgsym_init(void)
{
    return cstructures_init();
}

/* ------------------------------------------------------------------------- */
void
sfgsym_deinit(void)
{
    cstructures_deinit();
}
