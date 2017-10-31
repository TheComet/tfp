#pragma once

#include "tfp/config.hpp"
#include <QString>

namespace tfp {

class TFP_PUBLIC_API Config
{
public:
    void load();
    void save();

    QString currentLocale;

    struct {
        bool allowVariableExponents;
        bool analyticalExponents;
        bool exponentialExpansion;
        int expansionCount;
    } expressionSolver;
};

extern TFP_PUBLIC_API Config g_config;

}
