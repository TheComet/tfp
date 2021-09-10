#include "cstructures/backtrace.h"
#include <execinfo.h>

/* ------------------------------------------------------------------------- */
char**
get_backtrace(int* size)
{
    void* array[CSTRUCTURES_BACKTRACE_SIZE];
    char** strings;

    *size = backtrace(array, CSTRUCTURES_BACKTRACE_SIZE);
    strings = backtrace_symbols(array, *size);

    return strings;
}
