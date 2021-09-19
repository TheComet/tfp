#include "gmock/gmock.h"
#include "sfgsym/parsers/expression_parser.h"
#include "sfgsym/symbolic/expression.h"
#include "sfgsym/exporters/expr2dot.h"

#define NAME expression_parser

#define sfgsym_op_add reinterpret_cast<sfgsym_real (*)()>(sfgsym_op_add)
#define sfgsym_op_sub reinterpret_cast<sfgsym_real (*)()>(sfgsym_op_sub)
#define sfgsym_op_mul reinterpret_cast<sfgsym_real (*)()>(sfgsym_op_mul)
#define sfgsym_op_div reinterpret_cast<sfgsym_real (*)()>(sfgsym_op_div)
#define sfgsym_op_pow reinterpret_cast<sfgsym_real (*)()>(sfgsym_op_pow)

using namespace testing;

class NAME : public Test
{
public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
        if (e)
            sfgsym_expr_destroy_recurse(e);
    }

    sfgsym_expr* parse_string(const char* str)
    {
        sfgsym_expr_parser parser;
        sfgsym_expr_parser_init(&parser);
        sfgsym_expr* expr = sfgsym_expr_from_string(&parser, str);
        sfgsym_expr_parser_deinit(&parser);

        if (expr)
        {
            const testing::TestInfo* info = UnitTest::GetInstance()->current_test_info();
            std::string filename = std::string(info->test_suite_name()) + "__" + info->name() + ".dot";
            FILE* out = fopen(filename.c_str(), "w");
            sfgsym_export_expr_dot(expr, out);
            fclose(out);
        }

        return expr;
    }

    sfgsym_expr* e = nullptr;
};

TEST_F(NAME, collapse_subexpressions)
{
    e = parse_string("5(((x)))");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->parent, IsNull());
    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_mul));
    ASSERT_THAT(e->child[0], NotNull());
    EXPECT_THAT(e->child[0]->type, Eq(SFGSYM_LITERAL));
    EXPECT_THAT(e->child[0]->data.literal, DoubleEq(5.0));
    ASSERT_THAT(e->child[1], NotNull());
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->data.varname, StrEq("x"));
}

TEST_F(NAME, collapse_subexpressions_no_parent)
{
    e = parse_string("(((x)))");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->parent, IsNull());
    EXPECT_THAT(e->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->data.varname, StrEq("x"));
}

TEST_F(NAME, replace_exp_function)
{
    e = parse_string("exp(x)");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->parent, IsNull());
    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_pow));
    ASSERT_THAT(e->child[0], NotNull());
    EXPECT_THAT(e->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("e"));
    ASSERT_THAT(e->child[1], NotNull());
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->data.varname, StrEq("x"));
}
/*
TEST_F(NAME, replace_multiple_exp_functions)
{
    e = parse_string("a*exp(x)exp(b)c 2d+exp(e)^exp(f)");
    ASSERT_THAT(e, NotNull());
}*/

TEST_F(NAME, replace_infinity)
{
    e = parse_string("a*oo");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->parent, IsNull());
    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_mul));
    ASSERT_THAT(e->child[0], NotNull());
    EXPECT_THAT(e->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("a"));
    ASSERT_THAT(e->child[1], NotNull());
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_INFINITY));
}

TEST_F(NAME, simple_test)
{
    e = parse_string("a*b+3");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->parent, IsNull());
    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_add));
    ASSERT_THAT(e->child[0], NotNull());
    ASSERT_THAT(e->child[1], NotNull());
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_LITERAL));
    EXPECT_THAT(e->child[1]->data.literal, DoubleEq(3.0));
    EXPECT_THAT(e->child[0]->parent, Eq(e));
    EXPECT_THAT(e->child[1]->parent, Eq(e));
    sfgsym_expr* f = e->child[0];
    ASSERT_THAT(f->child[0], NotNull());
    ASSERT_THAT(f->child[1], NotNull());
    EXPECT_THAT(f->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(f->data.op, Eq(sfgsym_op_mul));
    EXPECT_THAT(f->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(f->child[0]->data.varname, StrEq("a"));
    EXPECT_THAT(f->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(f->child[1]->data.varname, StrEq("b"));
    EXPECT_THAT(f->child[0]->parent, Eq(f));
    EXPECT_THAT(f->child[1]->parent, Eq(f));
}

TEST_F(NAME, mul_before_add)
{
    e = parse_string("a+b*c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_add));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("a"));
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->data.op, Eq(sfgsym_op_mul));
    EXPECT_THAT(e->child[1]->child[0]->data.varname, StrEq("b"));
    EXPECT_THAT(e->child[1]->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, exponent_before_mul)
{
    e = parse_string("a*b^c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_mul));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("a"));
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->data.op, Eq(sfgsym_op_pow));
    EXPECT_THAT(e->child[1]->child[0]->data.varname, StrEq("b"));
    EXPECT_THAT(e->child[1]->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, exponent_before_add)
{
    e = parse_string("a+b^c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_add));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("a"));
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->data.op, Eq(sfgsym_op_pow));
    EXPECT_THAT(e->child[1]->child[0]->data.varname, StrEq("b"));
    EXPECT_THAT(e->child[1]->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, mul_before_sub)
{
    e = parse_string("a-b*c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_sub));
    ASSERT_THAT(e->child[0], NotNull());
    ASSERT_THAT(e->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("a"));
    ASSERT_THAT(e->child[1], NotNull());
    ASSERT_THAT(e->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->data.op, Eq(sfgsym_op_mul));
    ASSERT_THAT(e->child[1]->child[0], NotNull());
    ASSERT_THAT(e->child[1]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[0]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1]->child[1], NotNull());
    ASSERT_THAT(e->child[1]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, exponent_before_sub)
{
    e = parse_string("a-b^c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_sub));
    ASSERT_THAT(e->child[0], NotNull());
    ASSERT_THAT(e->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("a"));
    ASSERT_THAT(e->child[1], NotNull());
    ASSERT_THAT(e->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->data.op, Eq(sfgsym_op_pow));
    ASSERT_THAT(e->child[1]->child[0], NotNull());
    ASSERT_THAT(e->child[1]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[0]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1]->child[1], NotNull());
    ASSERT_THAT(e->child[1]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, div_before_add)
{
    e = parse_string("a+b/c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_add));
    ASSERT_THAT(e->child[0], NotNull());
    ASSERT_THAT(e->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("a"));
    ASSERT_THAT(e->child[1], NotNull());
    ASSERT_THAT(e->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->data.op, Eq(sfgsym_op_div));
    ASSERT_THAT(e->child[1]->child[0], NotNull());
    ASSERT_THAT(e->child[1]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[0]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1]->child[1], NotNull());
    ASSERT_THAT(e->child[1]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, exponent_before_div)
{
    e = parse_string("a/b^c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_div));
    ASSERT_THAT(e->child[0], NotNull());
    ASSERT_THAT(e->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("a"));
    ASSERT_THAT(e->child[1], NotNull());
    ASSERT_THAT(e->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->data.op, Eq(sfgsym_op_pow));
    ASSERT_THAT(e->child[1]->child[0], NotNull());
    ASSERT_THAT(e->child[1]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[0]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1]->child[1], NotNull());
    ASSERT_THAT(e->child[1]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, div_before_sub)
{
    e = parse_string("a-b/c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_sub));
    ASSERT_THAT(e->child[0], NotNull());
    ASSERT_THAT(e->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("a"));
    ASSERT_THAT(e->child[1], NotNull());
    ASSERT_THAT(e->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->data.op, Eq(sfgsym_op_div));
    ASSERT_THAT(e->child[1]->child[0], NotNull());
    ASSERT_THAT(e->child[1]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[0]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1]->child[1], NotNull());
    ASSERT_THAT(e->child[1]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, subexpression_before_mul)
{
    e = parse_string("(a+b)*c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_mul));
    ASSERT_THAT(e->child[0], NotNull());
    ASSERT_THAT(e->child[0]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[0]->data.op, Eq(sfgsym_op_add));
    ASSERT_THAT(e->child[0]->child[0], NotNull());
    ASSERT_THAT(e->child[0]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[0]->data.varname, StrEq("a"));
    ASSERT_THAT(e->child[0]->child[1], NotNull());
    ASSERT_THAT(e->child[0]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[1]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1], NotNull());
    ASSERT_THAT(e->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, subexpression_before_pow)
{
    e = parse_string("(a+b)^c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_pow));
    ASSERT_THAT(e->child[0], NotNull());
    ASSERT_THAT(e->child[0]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[0]->data.op, Eq(sfgsym_op_add));
    ASSERT_THAT(e->child[0]->child[0], NotNull());
    ASSERT_THAT(e->child[0]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[0]->data.varname, StrEq("a"));
    ASSERT_THAT(e->child[0]->child[1], NotNull());
    ASSERT_THAT(e->child[0]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[1]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1], NotNull());
    ASSERT_THAT(e->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, left_recursion_add)
{
    e = parse_string("a+b+c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_add));
    ASSERT_THAT(e->child[0], NotNull());
    EXPECT_THAT(e->child[0]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[0]->data.op, Eq(sfgsym_op_add));
    ASSERT_THAT(e->child[0]->child[0], NotNull());
    EXPECT_THAT(e->child[0]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[0]->data.varname, StrEq("a"));
    EXPECT_THAT(e->child[0]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[1]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1], NotNull());
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, left_recursion_sub)
{
    e = parse_string("a-b-c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_sub));
    ASSERT_THAT(e->child[0], NotNull());
    EXPECT_THAT(e->child[0]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[0]->data.op, Eq(sfgsym_op_sub));
    ASSERT_THAT(e->child[0]->child[0], NotNull());
    EXPECT_THAT(e->child[0]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[0]->data.varname, StrEq("a"));
    EXPECT_THAT(e->child[0]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[1]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1], NotNull());
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, left_recursion_mul)
{
    e = parse_string("a*b*c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_mul));
    ASSERT_THAT(e->child[0], NotNull());
    EXPECT_THAT(e->child[0]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[0]->data.op, Eq(sfgsym_op_mul));
    ASSERT_THAT(e->child[0]->child[0], NotNull());
    EXPECT_THAT(e->child[0]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[0]->data.varname, StrEq("a"));
    EXPECT_THAT(e->child[0]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[1]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1], NotNull());
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, left_recursion_div)
{
    e = parse_string("a/b/c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_div));
    ASSERT_THAT(e->child[0], NotNull());
    EXPECT_THAT(e->child[0]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[0]->data.op, Eq(sfgsym_op_div));
    ASSERT_THAT(e->child[0]->child[0], NotNull());
    EXPECT_THAT(e->child[0]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[0]->data.varname, StrEq("a"));
    EXPECT_THAT(e->child[0]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[1]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1], NotNull());
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, right_recursion_pow)
{
    e = parse_string("a^b^c");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_pow));
    ASSERT_THAT(e->child[0], NotNull());
    EXPECT_THAT(e->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("a"));
    ASSERT_THAT(e->child[1], NotNull());
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->data.op, Eq(sfgsym_op_pow));
    ASSERT_THAT(e->child[1]->child[0], NotNull());
    EXPECT_THAT(e->child[1]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[0]->data.varname, StrEq("b"));
    EXPECT_THAT(e->child[1]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[1]->data.varname, StrEq("c"));
}

TEST_F(NAME, implicit_mul_before_pow)
{
    e = parse_string("(a+b)c^d");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_mul));
    ASSERT_THAT(e->child[0], NotNull());
    EXPECT_THAT(e->child[0]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[0]->data.op, Eq(sfgsym_op_add));
    ASSERT_THAT(e->child[0]->child[0], NotNull());
    EXPECT_THAT(e->child[0]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[0]->data.varname, StrEq("a"));
    ASSERT_THAT(e->child[0]->child[1], NotNull());
    EXPECT_THAT(e->child[0]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->child[1]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1], NotNull());
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->data.op, Eq(sfgsym_op_pow));
    ASSERT_THAT(e->child[1]->child[0], NotNull());
    EXPECT_THAT(e->child[1]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[0]->data.varname, StrEq("c"));
    ASSERT_THAT(e->child[1]->child[1], NotNull());
    EXPECT_THAT(e->child[1]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[1]->data.varname, StrEq("d"));
}

TEST_F(NAME, implicit_mul_after_pow)
{
    e = parse_string("c^d(e+f)");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_pow));
    ASSERT_THAT(e->child[0], NotNull());
    EXPECT_THAT(e->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("c"));
    ASSERT_THAT(e->child[1], NotNull());
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->data.op, Eq(sfgsym_op_mul));
    ASSERT_THAT(e->child[1]->child[0], NotNull());
    EXPECT_THAT(e->child[1]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[0]->data.varname, StrEq("d"));
    ASSERT_THAT(e->child[1]->child[1], NotNull());
    EXPECT_THAT(e->child[1]->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->child[1]->data.op, Eq(sfgsym_op_add));
    ASSERT_THAT(e->child[1]->child[1]->child[0], NotNull());
    EXPECT_THAT(e->child[1]->child[1]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[1]->child[0]->data.varname, StrEq("e"));
    ASSERT_THAT(e->child[1]->child[1]->child[1], NotNull());
    EXPECT_THAT(e->child[1]->child[1]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[1]->child[1]->data.varname, StrEq("f"));
}

TEST_F(NAME, implicit_mul_and_pow_right_recursion)
{
    e = parse_string("a^b(c)^d(e)");
    ASSERT_THAT(e, NotNull());

    EXPECT_THAT(e->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->data.op, Eq(sfgsym_op_pow));
    ASSERT_THAT(e->child[0], NotNull());
    EXPECT_THAT(e->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[0]->data.varname, StrEq("a"));
    ASSERT_THAT(e->child[1], NotNull());
    EXPECT_THAT(e->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->data.op, Eq(sfgsym_op_mul));
    ASSERT_THAT(e->child[1]->child[0], NotNull());
    EXPECT_THAT(e->child[1]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[0]->data.varname, StrEq("b"));
    ASSERT_THAT(e->child[1]->child[1], NotNull());
    EXPECT_THAT(e->child[1]->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->child[1]->data.op, Eq(sfgsym_op_pow));
    ASSERT_THAT(e->child[1]->child[1]->child[0], NotNull());
    EXPECT_THAT(e->child[1]->child[1]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[1]->child[0]->data.varname, StrEq("c"));
    ASSERT_THAT(e->child[1]->child[1]->child[1], NotNull());
    EXPECT_THAT(e->child[1]->child[1]->child[1]->type, Eq(SFGSYM_OP + 1));
    EXPECT_THAT(e->child[1]->child[1]->child[1]->data.op, Eq(sfgsym_op_mul));
    ASSERT_THAT(e->child[1]->child[1]->child[1]->child[0], NotNull());
    EXPECT_THAT(e->child[1]->child[1]->child[1]->child[0]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[1]->child[1]->child[0]->data.varname, StrEq("d"));
    ASSERT_THAT(e->child[1]->child[1]->child[1]->child[1], NotNull());
    EXPECT_THAT(e->child[1]->child[1]->child[1]->child[1]->type, Eq(SFGSYM_VARIABLE));
    EXPECT_THAT(e->child[1]->child[1]->child[1]->child[1]->data.varname, StrEq("e"));
}

TEST_F(NAME, negate_before_mul)
{
    e = parse_string("-a*-b");
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, single_negate)
{
    e = parse_string("-a");
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, multiple_negate)
{
    e = parse_string("----a");
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, negate_before_subexpression)
{
    e = parse_string("-(a+b)");
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, negate_before_pow)
{
    e = parse_string("a^-b");
    ASSERT_THAT(e, NotNull());
}
