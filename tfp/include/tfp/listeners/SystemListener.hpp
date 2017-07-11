#pragma once

#include "tfp/config.hpp"

namespace tfp {

class SystemListener
{
public:
    virtual void onSystemStructureChanged() = 0;
    virtual void onSystemParametersChanged() = 0;
};

}
