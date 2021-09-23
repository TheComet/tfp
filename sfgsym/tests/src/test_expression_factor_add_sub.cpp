#include "gmock/gmock.h"
#include "sfgsym/exporters/expr2dot.h"
#include "sfgsym/parsers/expression_parser.h"
#include "sfgsym/symbolic/expression.h"

#define NAME expression_factor_add_sub

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
        {
            const testing::TestInfo* info = UnitTest::GetInstance()->current_test_info();
            std::string filename = std::string(info->test_suite_name()) + "__" + info->name() + "_2.dot";
            FILE* out = fopen(filename.c_str(), "w");
            sfgsym_export_expr_dot(e, out);
            fclose(out);
            sfgsym_expr_destroy_recurse(e);
        }
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
            std::string filename = std::string(info->test_suite_name()) + "__" + info->name() + "_1.dot";
            FILE* out = fopen(filename.c_str(), "w");
            sfgsym_export_expr_dot(expr, out);
            fclose(out);
        }

        return expr;
    }

    sfgsym_expr* e = nullptr;
};

TEST_F(NAME, a_plus_a)
{
    e = parse_string("a + a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, a_plus_b_plus_a_plus_b)
{
    e = parse_string("a + b + a + b");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, a_plus_b_plus_a)
{
    e = parse_string("a + b + a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, a_plus_three_a)
{
    e = parse_string("a + 3*a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, two_a_plus_a)
{
    e = parse_string("2*a + a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, two_a_plus_three_a)
{
    e = parse_string("2*a + 3*a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, a_plus_b_plus_c_plus_three_a)
{
    e = parse_string("a + b + c + 3*a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, b_plus_a_plus_c_plus_three_a)
{
    e = parse_string("b + a + c + 3*a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, two_a_plus_b_plus_c_plus_a)
{
    e = parse_string("2*a + b + c + a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, b_plus_two_a_plus_c_plus_a)
{
    e = parse_string("b + 2*a + c + a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, two_a_plus_b_plus_c_plus_three_a)
{
    e = parse_string("2*a + b + c + 3*a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, b_plus_two_a_plus_c_plus_three_a)
{
    e = parse_string("b + 2*a + c + 3*a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, long_chain_of_additions)
{
    e = parse_string("b+c+d+e+f+a+d+d+a+a+a+f");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, a_minus_a)
{
    e = parse_string("a-a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, a_minus_a_minus_a)
{
    e = parse_string("a-a-a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, a_minus_b_minus_a)
{
    e = parse_string("a-b-a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, long_chain_of_subtractions)
{
    e = parse_string("b-c-d-e-f-a-d-d-a-a-a-f");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, long_chain_of_additions_and_subtractions)
{
    e = parse_string("b-c+d-e-f+a+d-d-a+a-a+f+f-b+b-e-e+a-a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}

TEST_F(NAME, two_a_plus_a_plus_a)
{
    e = parse_string("2*a+a+a");
    ASSERT_THAT(e, NotNull());
    e = sfgsym_expr_collect_common_terms(e);
    ASSERT_THAT(e, NotNull());
}
