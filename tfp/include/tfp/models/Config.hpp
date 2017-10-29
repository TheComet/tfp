#pragma once

#include <QString>

namespace tfp {

class Config
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

extern Config g_config;

}
