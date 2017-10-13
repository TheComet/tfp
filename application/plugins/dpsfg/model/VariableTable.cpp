#include "model/VariableTable.hpp"

using namespace dpsfg;

// ----------------------------------------------------------------------------
void VariableTable::add(std::string name, double value)
{
    table_[name] = value;
}

// ----------------------------------------------------------------------------
void VariableTable::set(std::string name, double value)
{
    table_.at(name) = value;
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
double VariableTable::valueOf(std::string name) const
{
    return table_.at(name);
}
