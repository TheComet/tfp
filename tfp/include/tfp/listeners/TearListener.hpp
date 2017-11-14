#pragma once

#include "tfp/config.hpp"

namespace tfp {

class TearListener
{
public:
    virtual void onTears(const char* msg) = 0;
};

}
