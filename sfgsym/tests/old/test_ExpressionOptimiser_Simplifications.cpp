#include "gmock/gmock.h"
#include "tfp/math/Expression.hpp"
#include "tfp/math/VariableTable.hpp"
#include "tfp/util/Reference.hpp"
#include <random>

#define NAME ExpressionOptimiserSimplifications

using namespace testing;
using namespace tfp;

class NAME : public Test
{
public:
    NAME() : e(NULL) {}

    virtual void SetUp() override
    {
    }

    virtual void TearDown() override
    {
        ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
    }

    static void populateVTWithValues(VariableTable* vt)
    {
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(1,100);
        for (VariableTable::MappedValue entry : vt->mappedValues())
            vt->set(entry.first, distribution(generator));
    }

    void optimiseAndAssertBeforeAfter()
    {
        Reference<VariableTable> vt = e->generateVariableTable();
        populateVTWithValues(vt);

        double before = e->evaluate(vt);
        e->optimise();
        double after = e->evaluate(vt);
        ASSERT_THAT(before, DoubleEq(after));
    }

protected:
    Expression* e;
};

TEST_F(NAME, exponentiate_two_equal_product_operands)
{
    e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make("a"));
    e->dump("exponentiate_two_equal_product_operands.dot");
    optimiseAndAssertBeforeAfter();
    e->dump("exponentiate_two_equal_product_operands.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(2.0));
}

TEST_F(NAME, exponentiate_three_equal_product_operands)
{
    e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::mul,
            Expression::make("a"),
            Expression::make("a")));
    e->dump("exponentiate_three_equal_product_operands.dot");
    optimiseAndAssertBeforeAfter();
    e->dump("exponentiate_three_equal_product_operands.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(3.0));
}

TEST_F(NAME, exponentiate_chain_of_product_operands_with_constant_in_middle)
{
    e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::mul,
            Expression::make(5.0),
            Expression::make("a")));
    e->dump("exponentiate_chain_of_product_operands_with_constant_in_middle.dot");
    optimiseAndAssertBeforeAfter();
    e->dump("exponentiate_chain_of_product_operands_with_constant_in_middle.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->left()->name(), StrEq("a"));
    ASSERT_THAT(e->left()->right()->value(), DoubleEq(2.0));
    ASSERT_THAT(e->right()->value(), DoubleEq(5.0));
}

TEST_F(NAME, combine_exponents_simple_test)
{
    e = Expression::make(op::mul,
        Expression::make(op::pow,
            Expression::make("a"),
            Expression::make("x")),
        Expression::make(op::pow,
            Expression::make("a"),
            Expression::make("y")));
    e->dump("combine_exponents_simple_test.dot");
    optimiseAndAssertBeforeAfter();
    e->dump("combine_exponents_simple_test.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->left()->name(), StrEq("x"));
    ASSERT_THAT(e->right()->right()->name(), StrEq("y"));
}

TEST_F(NAME, combine_exponents_recursive_multiplies)
{
    e = Expression::make(op::mul,
        Expression::make(op::mul,
            Expression::make(op::pow,
                Expression::make("a"),
                Expression::make("x")),
            Expression::make("s")),
        Expression::make(op::mul,
            Expression::make(op::pow,
                Expression::make("a"),
                Expression::make("y")),
            Expression::make("t")));
    e->dump("combine_exponents_simple_test.dot");
    optimiseAndAssertBeforeAfter();
    e->dump("combine_exponents_simple_test.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->name(), StrEq("t"));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->right()->name(), StrEq("s"));
    ASSERT_THAT(e->left()->left()->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->left()->left()->name(), StrEq("a"));
    ASSERT_THAT(e->left()->left()->right()->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->left()->right()->left()->name(), StrEq("x"));
    ASSERT_THAT(e->left()->left()->right()->right()->name(), StrEq("y"));
}

TEST_F(NAME, simplify_addition_of_two_trees_that_are_same)
{
    e = Expression::make(op::add,
        Expression::make(op::pow,
            Expression::make("s"),
            Expression::make(4.0)),
        Expression::make(op::pow,
            Expression::make("s"),
            Expression::make(4.0)));
    e->dump("simplify_addition_of_two_trees_that_are_same.dot");
    optimiseAndAssertBeforeAfter();
    e->dump("simplify_addition_of_two_trees_that_are_same.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->left()->right()->value(), DoubleEq(4.0));
    ASSERT_THAT(e->right()->value(), DoubleEq(2.0));
}

TEST_F(NAME, simplify_identical_symbols_in_mul_and_div_chain_1)
{
    e = Expression::make(op::mul,
            Expression::make(op::div,
                Expression::make(1.0),
                Expression::make("a")),
            Expression::make("a"));
    e->dump("simplify_identical_symbols_in_mul_and_div_chain_1.dot");
    optimiseAndAssertBeforeAfter();
    e->dump("simplify_identical_symbols_in_mul_and_div_chain_1.dot", true);
    ASSERT_THAT(e->type(), Eq(Expression::CONSTANT));
    ASSERT_THAT(e->value(), DoubleEq(1.0));
}

TEST_F(NAME, simplify_identical_symbols_in_mul_and_div_chain_2)
{
    e = Expression::make(op::div,
            Expression::make(op::div,
                Expression::make("b"),
                Expression::make(op::mul,
                    Expression::make("c"),
                    Expression::make("a"))),
            Expression::make(op::div,
                Expression::make("d"),
                Expression::make(op::mul,
                    Expression::make("e"),
                    Expression::make("a"))));
    e->dump("simplify_identical_symbols_in_mul_and_div_chain_2.dot");
    optimiseAndAssertBeforeAfter();
    e->dump("simplify_identical_symbols_in_mul_and_div_chain_2.dot", true);

    ASSERT_THAT(e->find("a"), IsNull());
}

TEST_F(NAME, simplify_triple_division)
{
    e = Expression::make(op::div,
            Expression::make(op::div,
                Expression::make("c"),
                Expression::make(op::mul,
                    Expression::make("a"),
                    Expression::make("b"))),
            Expression::make(op::div,
                Expression::make("d"),
                Expression::make(op::mul,
                    Expression::make("e"),
                    Expression::make("a"))));
    e->dump("simplify_triple_division_1.dot");
    optimiseAndAssertBeforeAfter();
    e->dump("simplify_triple_division_1.dot", true);

    ASSERT_THAT(e->op2(), Eq(op::div));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->name(), StrEq("c"));
    ASSERT_THAT(e->left()->right()->name(), StrEq("e"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("b"));
    ASSERT_THAT(e->right()->right()->name(), StrEq("d"));

    ASSERT_THAT(e->find("a"), IsNull());
}
