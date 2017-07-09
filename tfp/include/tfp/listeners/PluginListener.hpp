#pragma once

#include "tfp/config.hpp"

namespace tfp {

class PluginListener
{
public:
    virtual void onPluginUnloading() = 0;
};

}
