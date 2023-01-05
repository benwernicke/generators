#include "string_check.h"

bool isnumber(const char* s)
{
    for (; isdigit(*s); ++s) { }
    return !*s;
}
