#include "gmock/gmock.h"
#include "tfp/math/Expression.hpp"
#include "tfp/util/Reference.hpp"

#define NAME Expression_optimiser

using namespace testing;
using namespace tfp;

TEST(NAME, simplify_constant_expressions)
{
    tfp::Reference<Expression> e = Expression::make(op::add,
        Expression::make(4.0),
        Expression::make(op::mul,
            Expression::make(2.0),
            Expression::make(3.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::CONSTANT));
    EXPECT_THAT(e->value(), DoubleEq(10.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, simplify_constant_expression_with_negate)
{
    Reference<Expression> e = Expression::make(op::pow,
        Expression::make("a"),
        Expression::make(op::negate,
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::FUNCTION2));
    ASSERT_THAT(e->right()->value(), DoubleEq(-1));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, simplify_constant_expressions_exponent)
{
    tfp::Reference<Expression> e = Expression::parse("2^5");
    ASSERT_THAT(e->type(), Eq(Expression::CONSTANT));
    EXPECT_THAT(e->value(), DoubleEq(32.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, simplify_constant_expressions_with_variables)
{
    tfp::Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::add,
            Expression::make(3.0),
            Expression::make(2.0)));
    e->optimise();
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->type(), Eq(Expression::CONSTANT));
    EXPECT_THAT(e->right()->value(), DoubleEq(5.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, add_variable_to_zero)
{
    tfp::Reference<Expression> e = Expression::make(op::add,
        Expression::make(op::add,
            Expression::make(op::add,
                Expression::make(0.0),
                Expression::make(0.0)),
            Expression::make(0.0)),
        Expression::make("a"));
    e->optimise();
    EXPECT_THAT(e->left(), IsNull());
    EXPECT_THAT(e->right(), IsNull());
    EXPECT_THAT(e->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, add_zero_to_variable)
{
    tfp::Reference<Expression> e = Expression::make(op::add,
        Expression::make(op::add,
            Expression::make(op::add,
                Expression::make("a"),
                Expression::make(0.0)),
            Expression::make(0.0)),
        Expression::make(0.0));
    e->optimise();
    EXPECT_THAT(e->left(), IsNull());
    EXPECT_THAT(e->right(), IsNull());
    EXPECT_THAT(e->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, subtract_zero_from_variable)
{
    tfp::Reference<Expression> e = Expression::make(op::sub,
        Expression::make("a"),
        Expression::make(op::sub,
            Expression::make(0.0),
            Expression::make(op::sub,
                Expression::make(0.0),
                Expression::make(0.0))));
    e->optimise();
    EXPECT_THAT(e->left(), IsNull());
    EXPECT_THAT(e->right(), IsNull());
    EXPECT_THAT(e->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, subtract_variable_from_zero)
{
    // 0 - (0 - (0 - a)) = -a
    tfp::Reference<Expression> e = Expression::make(op::sub,
        Expression::make(0.0),
        Expression::make(op::sub,
            Expression::make(0.0),
            Expression::make(op::sub,
                Expression::make(0.0),
                Expression::make("a"))));
    e->optimise();
    ASSERT_THAT(e->op1(), Eq(op::negate));
    ASSERT_THAT(e->left(), IsNull());
    ASSERT_THAT(e->right(), NotNull());
    ASSERT_THAT(e->right()->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, multiply_expression_by_one)
{
    tfp::Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::mul,
            Expression::make(1.0),
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, multiply_one_by_expression)
{
    tfp::Reference<Expression> e = Expression::make(op::mul,
        Expression::make(1.0),
        Expression::make(op::mul,
            Expression::make(1.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, divide_expression_by_one)
{
    tfp::Reference<Expression> e = Expression::make(op::div,
        Expression::make("a"),
        Expression::make(op::div,
            Expression::make(1.0),
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, multiply_expression_by_negative_one)
{
    tfp::Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::mul,
            Expression::make(1.0),
            Expression::make(-1.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::FUNCTION1));
    ASSERT_THAT(e->op1(), Eq(op::negate));
    ASSERT_THAT(e->left(), IsNull());
    ASSERT_THAT(e->right()->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, multiply_negative_one_by_expression)
{
    tfp::Reference<Expression> e = Expression::make(op::mul,
        Expression::make(1.0),
        Expression::make(op::mul,
            Expression::make(-1.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::FUNCTION1));
    ASSERT_THAT(e->op1(), Eq(op::negate));
    ASSERT_THAT(e->left(), IsNull());
    ASSERT_THAT(e->right()->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, divide_expression_by_negative_one)
{
    tfp::Reference<Expression> e = Expression::make(op::div,
        Expression::make("a"),
        Expression::make(op::div,
            Expression::make(1.0),
            Expression::make(-1.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::FUNCTION1));
    ASSERT_THAT(e->op1(), Eq(op::negate));
    ASSERT_THAT(e->left(), IsNull());
    ASSERT_THAT(e->right()->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, raise_expression_to_power_of_one)
{
    tfp::Reference<Expression> e = Expression::make(op::pow,
        Expression::make("a"),
        Expression::make(op::pow,
            Expression::make(1.0),
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::VARIABLE));
    ASSERT_THAT(e->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, optimise_addition_chain_with_variable_in_middle)
{
    Reference<Expression> e = Expression::make(op::add,
        Expression::make(1.0),
        Expression::make(op::add,
            Expression::make("a"),
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(2.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, optimise_subtraction_chain_with_variable_in_middle)
{
    Reference<Expression> e = Expression::make(op::sub,
        Expression::make(1.0),
        Expression::make(op::sub,
            Expression::make("a"),
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->op2(), Eq(op::sub));
    ASSERT_THAT(e->left()->value(), DoubleEq(2.0));
    ASSERT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, optimise_multiplication_chain_with_variable_in_middle)
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

TEST(NAME, optimise_division_chain_with_variable_in_middle)
{
    Reference<Expression> e = Expression::make(op::div,
        Expression::make(2.0),
        Expression::make(op::mul,
            Expression::make("a"),
            Expression::make(2.0)));
    e->optimise();
    ASSERT_THAT(e->left()->value(), DoubleEq(16.0));
    ASSERT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, optimise_addition_chain_with_variable_at_end)
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

TEST(NAME, optimise_subtraction_chain_with_variable_at_end)
{
    Reference<Expression> e = Expression::make(op::sub,
        Expression::make(2.0),
        Expression::make(op::sub,
            Expression::make(1.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->value(), DoubleEq(1.0));
    ASSERT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, optimise_multiplication_chain_with_variable_at_end)
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

TEST(NAME, optimise_division_chain_with_variable_at_end)
{
    Reference<Expression> e = Expression::make(op::div,
        Expression::make(2.0),
        Expression::make(op::mul,
            Expression::make(2.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->left()->value(), DoubleEq(4.0));
    ASSERT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, optimise_recursive_redundant_additions)
{
    Reference<Expression> e = Expression::make(op::add,
        Expression::make(op::add,
            Expression::make(5.0),
            Expression::make("a")),
        Expression::make(op::add,
            Expression::make(3.0),
            Expression::make("b")));
    e->dump("optimise_recursive_redundant_additions.dot");
    e->optimise();
    e->dump("optimise_recursive_redundant_additions.dot", true);

    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->name(), StrEq("b"));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->left()->value(), DoubleEq(8.0));
    ASSERT_THAT(e->left()->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, optimise_recursive_redundant_multiplications)
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

TEST(NAME, exponentiate_two_equal_product_operands)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make("a"));
    e->dump("exponentiate_two_equal_product_operands.dot");
    e->optimise();
    e->dump("exponentiate_two_equal_product_operands.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(2.0));
}

TEST(NAME, exponentiate_three_equal_product_operands)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::mul,
            Expression::make("a"),
            Expression::make("a")));
    e->dump("exponentiate_three_equal_product_operands.dot");
    e->optimise();
    e->dump("exponentiate_three_equal_product_operands.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(3.0));
}

TEST(NAME, exponentiate_chain_of_product_operands_with_constant_in_middle)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::mul,
            Expression::make(5.0),
            Expression::make("a")));
    e->dump("exponentiate_chain_of_product_operands_with_constant_in_middle.dot");
    e->optimise();
    e->dump("exponentiate_chain_of_product_operands_with_constant_in_middle.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->value(), DoubleEq(5.0));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(2.0));
}

TEST(NAME, collapse_chain_of_3_add_operations)
{
    Reference<Expression> e = Expression::make(op::add,
        Expression::make(5.0),
        Expression::make(op::add,
            Expression::make("a"),
            Expression::make(op::add,
                Expression::make("b"),
                Expression::make(8.0))));
    e->dump("collapse_chain_of_3_add_operations.dot");
    e->recursivelyCall(&Expression::collapseChainOfOperations);
    e->dump("collapse_chain_of_3_add_operations.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->left()->name(), StrEq("b"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(13.0));
}

TEST(NAME, collapse_chain_of_3_mul_operations)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make(3.0),
        Expression::make(op::mul,
            Expression::make("a"),
            Expression::make(op::mul,
                Expression::make("b"),
                Expression::make(2.0))));
    e->dump("collapse_chain_of_3_mul_operations.dot");
    e->recursivelyCall(&Expression::collapseChainOfOperations);
    e->dump("collapse_chain_of_3_mul_operations.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->left()->name(), StrEq("b"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(6.0));
}
