#pragma once

#include "tfp/util/RefCounted.hpp"
#include <unordered_map>

namespace dpsfg {

class VariableTable : public tfp::RefCounted
{
public:
    void add(std::string name, double value);
    void set(std::string name, double value);
    void remove(std::string name);
    void clear();
    double valueOf(std::string name) const;

private:
    std::unordered_map<std::string, double> table_;
};

}
