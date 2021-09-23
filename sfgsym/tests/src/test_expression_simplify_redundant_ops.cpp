#include "gmock/gmock.h"
#include "sfgsym/exporters/expr2dot.h"
#include "sfgsym/parsers/expression_parser.h"
#include "sfgsym/symbolic/expression.h"

#define NAME expression_simplify_redundant_ops

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

/*
TEST_F(NAME, constant_addition_chain_with_variable_in_middle)
{
    e = parse_string("1+a+1");
    ASSERT_THAT(e, NotNull());

    ASSERT_THAT(sfgsym_expr_simplify_redudnant_adds(e), Gt(0));

    ASSERT_THAT(e->parent, IsNull());
    ASSERT_THAT(e->type, Eq(SFGSYM_OP + 1));
    ASSERT_THAT(e->data.op, Eq(sfgsym_op_add));
    ASSERT_THAT(e->child[0], NotNull());
    ASSERT_THAT(e->child[0]->parent, Eq(e));
    ASSERT_THAT(e->child[0]->type, Eq(SFGSYM_VARIABLE));
    ASSERT_THAT(e->child[0]->data.varname, StrEq("a"));
    ASSERT_THAT(e->child[1], NotNull());
    ASSERT_THAT(e->child[1]->parent, Eq(e));
    ASSERT_THAT(e->child[1]->type, Eq(SFGSYM_LITERAL));
    ASSERT_THAT(e->child[1]->data.literal, DoubleEq(2.0));
}

TEST_F(NAME, constant_subtraction_chain_with_variable_in_middle)
{
    Reference<Expression> e = Expression::make(op::sub,
        Expression::make(1.0),
        Expression::make(op::sub,
            Expression::make("a"),
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->op2(), Eq(op::sub));
    ASSERT_THAT(e->left()->type(), Eq(Expression::VARIABLE));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(2.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST_F(NAME, constant_multiplication_chain_with_variable_in_middle)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make(2.0),
        Expression::make(op::mul,
            Expression::make("a"),
            Expression::make(2.0)));
    e->dump("optimise_multiplication_chain_with_variable_in_middle.dot");
    e->optimise();
    e->dump("optimise_multiplication_chain_with_variable_in_middle.dot", true);
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(4.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST_F(NAME, constant_division_chain_with_variable_in_middle)
{
    Reference<Expression> e = Expression::make(op::div,
        Expression::make(2.0),
        Expression::make(op::mul,
            Expression::make("a"),
            Expression::make(2.0)));
    e->optimise();
    ASSERT_THAT(e->left()->type(), Eq(Expression::VARIABLE));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(16.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST_F(NAME, constant_addition_chain_with_variable_at_end)
{
    Reference<Expression> e = Expression::make(op::add,
        Expression::make(1.0),
        Expression::make(op::add,
            Expression::make(1.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->left()->value(), DoubleEq(2.0));
    ASSERT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST_F(NAME, constant_subtraction_chain_with_variable_at_end)
{
    Reference<Expression> e = Expression::make(op::sub,
        Expression::make(2.0),
        Expression::make(op::sub,
            Expression::make(1.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(1.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST_F(NAME, constant_multiplication_chain_with_variable_at_end)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make(2.0),
        Expression::make(op::mul,
            Expression::make(2.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->left()->value(), DoubleEq(4.0));
    ASSERT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST_F(NAME, constant_division_chain_with_variable_at_end)
{
    Reference<Expression> e = Expression::make(op::div,
        Expression::make(2.0),
        Expression::make(op::mul,
            Expression::make(2.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->left()->type(), Eq(Expression::VARIABLE));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(4.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST_F(NAME, constant_recursive_redundant_additions)
{
    Reference<Expression> e = Expression::make(op::add,
        Expression::make(op::add,
            Expression::make(5.0),
            Expression::make("a")),
        Expression::make(op::add,
            Expression::make(3.0),
            Expression::make("b")));
    e->dump("constant_recursive_redundant_additions.dot");
    e->optimise();
    e->dump("constant_recursive_redundant_additions.dot", true);

    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->name(), StrEq("b"));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->left()->value(), DoubleEq(8.0));
    ASSERT_THAT(e->left()->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST_F(NAME, constant_recursive_redundant_multiplications)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make(op::mul,
            Expression::make(3.0),
            Expression::make("a")),
        Expression::make(op::mul,
            Expression::make(5.0),
            Expression::make("b")));
    e->dump("optimise_recursive_redundant_multiplications.dot");
    e->optimise();
    e->dump("optimise_recursive_redundant_multiplications.dot", true);

    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->name(), StrEq("b"));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->value(), DoubleEq(15.0));
    ASSERT_THAT(e->left()->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST_F(NAME, constant_chain_of_additions_with_2_variables_in_middle)
{
    Reference<Expression> e = Expression::make(op::add,
        Expression::make(5.0),
        Expression::make(op::add,
            Expression::make("a"),
            Expression::make(op::add,
                Expression::make("b"),
                Expression::make(8.0))));
    e->dump("constant_chain_of_additions_with_2_variables_in_middle.dot");
    e->optimise();
    e->dump("constant_chain_of_additions_with_2_variables_in_middle.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->left()->name(), StrEq("b"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(13.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST_F(NAME, constant_chain_of_products_with_2_variables_in_middle)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make(3.0),
        Expression::make(op::mul,
            Expression::make("a"),
            Expression::make(op::mul,
                Expression::make("b"),
                Expression::make(2.0))));
    e->dump("constant_chain_of_products_with_2_variables_in_middle.dot");
    e->optimise();
    e->dump("constant_chain_of_products_with_2_variables_in_middle.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->left()->name(), StrEq("b"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(6.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}*/
