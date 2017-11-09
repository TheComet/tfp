#pragma once

#include "tfp/config.hpp"
#include <stdexcept>

namespace tfp {

class TFP_PUBLIC_API NonConstantExponentException : public std::logic_error
{
public:
    NonConstantExponentException(const char* msg) : std::logic_error(
        std::string("Variable depends on a non-constant expression! ") + msg
    ) {}
};

class TFP_PUBLIC_API MissingEntryException : public std::runtime_error
{
public:
    MissingEntryException(const char* key) : std::logic_error(
        std::string("Entry \"") + key + "\" does not exist in variable table"
    ) {}
};

class TFP_PUBLIC_API CyclicDependencyException : public std::runtime_error
{
public:
    CyclicDependencyException(const char* key) : std::logic_error(
        std::string("Cyclic dependency of entries was detected while looking up key \"") + key + "\""
    ) {}
};

}
