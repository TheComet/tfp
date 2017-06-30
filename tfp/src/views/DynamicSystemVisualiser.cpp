#include "tfp/views/DynamicSystemVisualiser.hpp"
#include "tfp/views/DynamicSystemConfig.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
DynamicSystemVisualiser::DynamicSystemVisualiser(QWidget* parent) :
    QWidget(parent),
    system_(NULL)
{
}

// ----------------------------------------------------------------------------
void DynamicSystemVisualiser::setSystem(DynamicSystemConfig* system)
{
    if (system_ != NULL)
    {
        disconnect(system_, SIGNAL(parametersChanged()), this, SLOT(onParametersChanged()));
        disconnect(system_, SIGNAL(structureChanged()), this, SLOT(onStructureChanged()));
    }

    system_ = system;

    if (system_ != NULL)
    {
        connect(system_, SIGNAL(parametersChanged()), this, SLOT(onParametersChanged()));
        connect(system_, SIGNAL(structureChanged()), this, SLOT(onStructureChanged()));
        onStructureChanged();
        onParametersChanged();
    }
}

// ----------------------------------------------------------------------------
void DynamicSystemVisualiser::onParametersChanged()
{
    onSystemParametersChanged();
}

// ----------------------------------------------------------------------------
void DynamicSystemVisualiser::onStructureChanged()
{
    onSystemStructureChanged();
}

}
