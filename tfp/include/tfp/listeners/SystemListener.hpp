#pragma once

#include "tfp/config.hpp"

namespace tfp {

class SystemListener
{
public:
    virtual void onSystemParametersChanged() = 0;
    virtual void onSystemStructureChanged() = 0;
};

}
