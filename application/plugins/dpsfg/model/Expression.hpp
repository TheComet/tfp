#pragma once

#include "tfp/util/RefCounted.hpp"
#include <string>
#include <vector>

namespace dpsfg {

class Expression : public tfp::RefCounted
{
    Expression(const Expression& other);

public:
    Expression();
    Expression(const char* expression)  { *this = expression; }
    ~Expression();

    Expression& operator=(const char* expression);
    Expression& operator=(const Expression& other);
    Expression& operator+=(Expression* rhs);
    Expression& operator-=(Expression* rhs);
    Expression& operator*=(Expression* rhs);
    Expression& operator/=(Expression* rhs);
    Expression& operator^=(Expression* rhs);

private:
    Expression* parent_;
    Expression* left_;
    Expression* right_;
    std::string symbolName_;
    char operation_;
};

//inline Expression operator+(Expression lhs, const Expression& rhs) { lhs += rhs; return lhs;}
//inline Expression operator-(Expression lhs, const Expression& rhs) { lhs -= rhs; return lhs;}
//inline Expression operator*(Expression lhs, const Expression& rhs) { lhs *= rhs; return lhs;}
//inline Expression operator/(Expression lhs, const Expression& rhs) { lhs /= rhs; return lhs;}
//inline Expression operator^(Expression lhs, const Expression& rhs) { lhs ^= rhs; return lhs;}

}
