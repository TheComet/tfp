#pragma once

#include "tfp/util/Reference.hpp"
#include <vector>
#include <unordered_map>
#include <set>

namespace dpsfg {

class Expression;

class VariableTable : public tfp::RefCounted
{
public:
    typedef std::vector< std::pair<std::string, double> > ValueVariables;
    typedef std::vector< std::pair<std::string, std::string> > ExpressionVariables;

    void add(std::string name, double value);
    void add(std::string name, const char* expression);
    void set(std::string name, double value);
    void set(std::string name, const char* expression);
    Expression* get(const std::string& name) const;
    void remove(std::string name);
    void clear();

    /*!
     * @brief Merges variables from another table. If variables in the other
     * table already exist in this table, then they are overwritten.
     */
    void merge(const VariableTable& source);

    /*!
     * @brief Recursively evaluates the expression the specified variable maps
     * to until a constant value is computed.
     */
    double valueOf(std::string name) const;
    double valueOf(std::string name, std::set<std::string>* visited) const;

    /*!
     * @brief Generates an alphabetically sorted list of all entries that have
     * direct values.
     */
    ValueVariables valueVariableList() const;

    /*!
     * @brief Generates an alphabetically sorted list of all entries that have
     * further expressions as their value.
     */
    ExpressionVariables expressionVariableList() const;

private:
    typedef std::unordered_map<std::string, tfp::Reference<Expression> > Table;
    Table table_;
};

}
