#include "model/VariableTable.hpp"
#include "model/Expression.hpp"

using namespace dpsfg;

// ----------------------------------------------------------------------------
void VariableTable::add(std::string name, double value)
{
    table_[name] = Expression::make(value);
}

// ----------------------------------------------------------------------------
void VariableTable::add(std::string name, const char* expression)
{
    Expression* e = Expression::parse(expression);
    if (table_.insert(std::pair< std::string, tfp::Reference<Expression> >(
        name, e
    )).second == false)
    {
        throw std::runtime_error("Tried adding variable \"" + name + "\" but it already exists!");
    }

    /*
     * If the expression was not a constant, it's possible that further
     * variables were introduced.
     */
    if (e->type() != Expression::CONSTANT)
    {
        tfp::Reference<VariableTable> vt = e->generateVariableTable();
        merge(*vt);
    }
}

// ----------------------------------------------------------------------------
void VariableTable::set(std::string name, double value)
{
    table_.at(name)->set(value);
}

// ----------------------------------------------------------------------------
void VariableTable::set(std::string name, const char* expression)
{
    Expression* e = Expression::parse(expression);
    table_.at(name) = e;

    /*
     * If the expression was not a constant, it's possible that further
     * variables were introduced.
     */
    if (e->type() != Expression::CONSTANT)
    {
        tfp::Reference<VariableTable> vt = e->generateVariableTable();
        merge(*vt);
    }
}

// ----------------------------------------------------------------------------
Expression* VariableTable::get(const std::string& name) const
{
    return table_.at(name);
}

// ----------------------------------------------------------------------------
void VariableTable::remove(std::string name)
{
    table_.erase(name);
}

// ----------------------------------------------------------------------------
void VariableTable::clear()
{
    table_.clear();
}

// ----------------------------------------------------------------------------
void VariableTable::merge(const VariableTable& other)
{
    table_.insert(other.table_.begin(), other.table_.end());
}

// ----------------------------------------------------------------------------
double VariableTable::valueOf(std::string name) const
{
    std::set<std::string> visited;
    return valueOf(name, &visited);
}

// ----------------------------------------------------------------------------
double VariableTable::valueOf(std::string name, std::set<std::string>* visited) const
{
    Expression* e = table_.at(name);
    if (e->type() == Expression::VARIABLE && visited->insert(name).second == false)
        throw std::runtime_error("Error: cyclic expression dependency detected while looking up value of variable");
    return e->evaluate(this, visited);
}
