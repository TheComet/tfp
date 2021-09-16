#include "sfgsym/parsers/expression_parser.h"
#include "sfgsym/parsers/expression_parser.l.h"
#include "sfgsym/parsers/expression_parser_post.h"
#include "sfgsym/symbolic/expression.h"
#include "sfgsym/util/log.h"
#include <string.h>
#include <stdarg.h>
#include <assert.h>

/* ------------------------------------------------------------------------- */
static struct sfgsym_expr*
do_parse(struct sfgsym_expr_parser* driver)
{
    SFGSYMSTYPE pushed_value;
    int pushed_char;
    int parse_result;
    SFGSYMLTYPE loc = {1, 1, 1, 1};

    do
    {
        pushed_char = sfgsymlex(&pushed_value, &loc, driver->scanner);
        parse_result = sfgsympush_parse(driver->parser, pushed_char, &pushed_value, &loc, driver->scanner);
    } while (parse_result == YYPUSH_MORE);

    if (parse_result == 0 && driver->result)
    {
        struct sfgsym_expr* expr = driver->result;
        driver->result = NULL;
        return expr;
    }

    return NULL;
}

/* ------------------------------------------------------------------------- */
static struct sfgsym_expr*
do_post_parse(struct sfgsym_expr_parser* driver, struct sfgsym_expr* expr)
{
    struct sfgsym_expr* result;
    result = sfgsym_expr_parser_replace_exp_function(driver, expr);
    if (result == NULL)
        goto fail;
    expr = result;

    result = sfgsym_expr_parser_collapse_lists(driver, expr);
    if (result == NULL)
        goto fail;
    expr = result;

    return expr;

    fail : sfgsym_expr_destroy_recurse(expr);
    return NULL;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_expr_parser_init(struct sfgsym_expr_parser* driver)
{
    if (sfgsymlex_init_extra(driver, &driver->scanner) != 0)
    {
        sfgsym_log("eFailed to initialize FLEX scanner\n");
        goto init_scanner_failed;
    }

    driver->parser = sfgsympstate_new();
    if (driver->parser == NULL)
    {
        sfgsym_log("eFailed to initialize BISON parser\n");
        goto init_parser_failed;
    }

    driver->result = NULL;
    driver->log = sfgsym_log;

    return 0;

    init_parser_failed  : sfgsymlex_destroy(driver->scanner);
    init_scanner_failed : return -1;
}

/* ------------------------------------------------------------------------- */
void
sfgsym_expr_parser_deinit(struct sfgsym_expr_parser* driver)
{
    sfgsympstate_delete(driver->parser);
    sfgsymlex_destroy(driver->scanner);
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_expr_from_string(struct sfgsym_expr_parser* driver, const char* str)
{
    return sfgsym_expr_from_buffer(driver, str, (int)strlen(str));
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_expr_from_buffer(struct sfgsym_expr_parser* driver, const void* bytes, int len)
{
    struct sfgsym_expr* result;

    YY_BUFFER_STATE buf = sfgsym_scan_bytes(bytes, len, driver->scanner);
    if (buf == NULL)
    {
        sfgsym_log("eFailed to allocate FLEX buffer state\n");
        return NULL;
    }

    result = do_parse(driver);
    sfgsym_delete_buffer(buf, driver->scanner);

    if (result == NULL)
        return NULL;

    result = do_post_parse(driver, result);
    return result;
}

/* ------------------------------------------------------------------------- */
void
sfgsym_expr_parser_give_result(struct sfgsym_expr_parser* driver, struct sfgsym_expr* expr)
{
    assert(driver->result == NULL);
    driver->result = expr;
}

/* ------------------------------------------------------------------------- */
void
sfgsymerror(SFGSYMLTYPE* locp, sfgsymscan_t scanner, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    sfgsym_vlog_error(fmt, ap);
    va_end(ap);
    sfgsym_log("\n");
}
