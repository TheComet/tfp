#pragma once

#include "sfgsym/config.h"
#include <stdio.h>

C_BEGIN

struct sfgsym_expr;

SFGSYM_PUBLIC_API int
sfgsym_export_expr_dot_file(const struct sfgsym_expr* expr, const char* file_name);

SFGSYM_PUBLIC_API int
sfgsym_export_expr_dot(const struct sfgsym_expr* expr, FILE* fp);

C_END
